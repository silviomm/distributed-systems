#include "mysocket.h"
#include <pthread.h>

#define NTHREADS 3
#define TAM_BUFFER 100

pthread_mutex_t lock;
int client_active = 0;

/* Structure of arguments to pass to client thread */
struct TArgs {
  TSocket cliSock;   /* socket descriptor for client */
};

typedef struct Buffer {
  char* buff[TAM_BUFFER];
  int pos;
} Buffer;

void consume(Buffer** b);

void * HandleRequest(void *args);

void * Response(void *args);

void conecta_servidor_usuarios(int porta);

int chat_active();

void change_active(int n);

int main(int argc, char *argv[]) {
  TSocket srvSock, cliSock;        /* server and client sockets */
  struct TArgs *args_req;              /* argument structure for thread */
  struct TArgs *args_resp;
  
  if (argc == 1) { ExitWithError("Usage: server <local port>"); }

  /* inicia mutex */
  if (pthread_mutex_init(&lock, NULL) != 0) ExitWithError("\n lock init failed\n");

  /* Create a passive-mode listener endpoint */
  srvSock = CreateServer(atoi(argv[1]));

  /* Conecta com servidor de usuários e se registra */
  //conecta_servidor_usuarios(atoi(argv[1]));

  pthread_t thread_req, thread_resp;
  while(1) {

    cliSock = AcceptConnection(srvSock);
    change_active(1);
    
    if ((args_req = (struct TArgs *) malloc(sizeof(struct TArgs))) == NULL) ExitWithError("args_req malloc() failed");
    else args_req->cliSock = cliSock;

    if ((args_resp = (struct TArgs *) malloc(sizeof(struct TArgs))) == NULL) ExitWithError("args_resp malloc() failed");
    else args_resp->cliSock = cliSock;

    /* Create a new thread to handle the client requests */
    if (pthread_create(&thread_req, NULL, HandleRequest, (void *) args_req)) ExitWithError("req pthread_create() failed");
    if (pthread_create(&thread_resp, NULL, Response, (void *) args_resp)) ExitWithError("resp pthread_create() failed");

    pthread_join(thread_req, NULL);
    pthread_join(thread_resp, NULL);

    printf("final\n");

  }

  /* NOT REACHED */
}


void consume(Buffer** b) {
  for(int i=0; i < (*b)->pos; i++) {
    printf("%s", (*b)->buff[i]);
  }
  (*b)->pos = 0;
}

void produce(char* str, Buffer** b) {
  if ((*b)->pos >= TAM_BUFFER) (*b)->pos = 0;
  (*b)->buff[(*b)->pos++] = str;
}


/* Handle client request */
void * HandleRequest(void *args) {
  char str[100];
  TSocket cliSock;

  /* Create Buffer and set initial position */
  Buffer* b = (struct Buffer*) malloc(sizeof(Buffer));
  b->pos = 0;

  /* Extract socket file descriptor and deallocate memory from argument */
  cliSock = ((struct TArgs *) args) -> cliSock;
  free(args);

  for(;;) {
    /* Receive the request */
    if (ReadLine(cliSock, str, 99) < 0) { 
      ExitWithError("ReadLine() failed");
    } 
    else {
      printf("recebi isso: %s\n", str);
      produce(str, &b);
    }

    
    /* Consome a msg apenas se chat não terminou */
    if(!chat_active()) break;
    consume(&b);
    
    if (strncmp(str, "/FIM", 3) == 0) {
      change_active(0);
      break;
    }
  }

  close(cliSock);
  pthread_exit(NULL);
}

void * Response(void *args) {
  char response[100];
  TSocket cliSock;

  /* Extract socket file descriptor and deallocate memory from argument */
  cliSock = ((struct TArgs *) args) -> cliSock;
  free(args);

  for(;;) {

    scanf("%s", response);
    
    sprintf(response, "%s\n", response);
    
    if(!chat_active()) break;
    
    if (WriteN(cliSock, response, strlen(response)) <= 0) { 
      ExitWithError("WriteN() failed"); 
    }
    
    if (strncmp(response, "/FIM", 3) == 0) {
      change_active(0);
      break;
    }
  
  }

  pthread_exit(NULL);
}

int chat_active() {
  pthread_mutex_lock(&lock);
    int aux = client_active;
  pthread_mutex_unlock(&lock);
  return aux;
}

void change_active(int n) {
  pthread_mutex_lock(&lock);
    client_active = n;
  pthread_mutex_unlock(&lock);
}


/* Conecta com servidor de usuários e se registra */
void conecta_servidor_usuarios(int porta) {
  
  printf("Diga o IP do servidor de usuários...\n");
  char chatServer[20];
  scanf("%s", chatServer);
  TSocket sock = ConnectToServer(chatServer, 2018); //2018 porta padrão do projeto
  
  printf("Diga seu nome de usuario...\n");
  char username[30];
  scanf("%s", username);

  // registra usuário
  sprintf(username, "1 %s %d \n", username, porta);
  if (WriteN(sock, username, strlen(username)) <= 0) { 
    ExitWithError("WriteN() failed"); 
  }

  char resposta[20];
  if (ReadLine(sock, resposta, 99) < 0) { 
    ExitWithError("ReadLine() failed");
  }
  else {
    // confirmações 0, 1, 2?
  }

}

