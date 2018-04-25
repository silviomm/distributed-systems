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

char username[30];

void consume(Buffer** b);

void * HandleRequest(void *args);

void * Response(void *args);

void conecta_servidor_usuarios(char* ip);

void desconecta_servidor_usuarios(char* ip);

int chat_active();

void change_active(int n);

pthread_t thread_req, thread_resp;

int main(int argc, char *argv[]) {
  TSocket srvSock, cliSock;       /* server and client sockets */
  struct TArgs *args_req;         /* argument structure for thread */
  struct TArgs *args_resp;        /* argument structure for thread */
  fd_set set; int ret;            /* preparando select */
  char str[100];                  /* string pra comando usuario */

  if (argc != 3) { ExitWithError("Usage: server <local port> <server ip>"); }

  /* inicia mutex */
  if (pthread_mutex_init(&lock, NULL) != 0) ExitWithError("\n lock init failed\n");

  /* Create a passive-mode listener endpoint */
  srvSock = CreateServer(atoi(argv[1]));

  /* Conecta com servidor de usuários e se registra */
  conecta_servidor_usuarios(argv[2]);

  while(1) {

    /* Initialize the file descriptor set */
    FD_ZERO(&set);
    /* Include stdin into the file descriptor set */
    FD_SET(STDIN_FILENO, &set);
    /* Include srvSock into the file descriptor set */
    FD_SET(srvSock, &set);

    ret = select (FD_SETSIZE, &set, NULL, NULL, NULL);
    if (ret<0) {
       WriteError("select() failed");
       break;
    }

    /* Read from stdin */
    if (FD_ISSET(STDIN_FILENO, &set)) {
      scanf("%s", str);
      if (strncmp(str, "FIM", 3) == 0) {
        desconecta_servidor_usuarios(argv[2]);
        break;
      }
    }

    /* Read from srvSock */
    if (FD_ISSET(srvSock, &set)) {
      printf("Oba! Nova conexão!\n");
      cliSock = AcceptConnection(srvSock);
      change_active(1);
      
      /* aloca estruturas para as threads */
      if ((args_req = (struct TArgs *) malloc(sizeof(struct TArgs))) == NULL) ExitWithError("args_req malloc() failed");
      else args_req->cliSock = cliSock;

      if ((args_resp = (struct TArgs *) malloc(sizeof(struct TArgs))) == NULL) ExitWithError("args_resp malloc() failed");
      else args_resp->cliSock = cliSock;

      /* cria thread de request e response */
      if (pthread_create(&thread_req, NULL, HandleRequest, (void *) args_req)) ExitWithError("req pthread_create() failed");
      if (pthread_create(&thread_resp, NULL, Response, (void *) args_resp)) ExitWithError("resp pthread_create() failed");

      pthread_join(thread_req, NULL);
      pthread_join(thread_resp, NULL);
    }

    

    printf("Sua conversa acabou. Digite /FIM para sair ou espere uma nova conexão...\n");

  }

  printf("fim\n");

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
      ExitWithError("ReadLine() HandleRequest failed");
    } 
    else {
      produce(str, &b);
    }

    
    /* Consome a msg apenas se chat não terminou */
    if(!chat_active()) break;
    consume(&b);
    
    if (strncmp(str, "FIM", 3) == 0) {
      change_active(0);
      pthread_cancel(thread_resp);
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
      ExitWithError("WriteN() Response failed"); 
    }
    
    if (strncmp(response, "FIM", 3) == 0) {
      change_active(0);
      pthread_cancel(thread_req);
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
void conecta_servidor_usuarios(char* ip) {
  
  printf("Conectando ao servidor de usuários...\n");
  TSocket sock = ConnectToServer(ip, 2018); //2018 porta padrão do projeto
  
  printf("Diga seu nome de usuario...\n");
  scanf("%s", username);

  // registra usuário
  char str[100];
  sprintf(str, "1 %s %d \n", username, 8080);
  if (WriteN(sock, str, strlen(str)) <= 0) { 
    ExitWithError("WriteN() conecta_servidor_usuarios failed"); 
  }

  char resposta[20];
  if (ReadLine(sock, resposta, 99) < 0) { 
    ExitWithError("ReadLine() conecta_servidor_usuarios failed");
  }
  else {
    int id = atoi(resposta);
    switch(id) {
      case 0:
        printf("servidor cheio!\n");
        break;
      case 1:
        printf("login já existente!\n");
        break;
      case 2:
        printf("cadastrado com sucesso!\n");
        break;
      default:
        break;
    }
  }
}

void desconecta_servidor_usuarios(char* ip) {
  
  printf("Desonectando do servidor de usuários...\n");
  TSocket sock = ConnectToServer(ip, 2018); //2018 porta padrão do projeto
  
  char str[100];
  sprintf(str, "3 %s \n", username);
  if (WriteN(sock, str, strlen(str)) <= 0) { 
    ExitWithError("WriteN() desconecta_servidor_usuarios failed"); 
  }
}

