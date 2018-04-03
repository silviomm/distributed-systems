#include "mysocket.h"
#include <pthread.h>

#define NTHREADS 5

/*
  fd-set set; variavel set
  fd-zero iniciar as coisas
  fd-set() setar os descritores q serao escutados
  fd-isset(desc, &set) verificar se o descritor ta la
  select(fd-setsize, &set, nul, null, null)
*/

int cont_thread = 0;
pthread_mutex_t lock, socketLock;
TSocket activeSocket = 0;

/* Structure of arguments to pass to client thread */
struct TArgs {
  TSocket cliSock;   /* socket descriptor for client */
  int id;
};

typedef struct Buffer {
  char* buff[100];
  int pos;
} Buffer;
Buffer b1, b2, b3, b4, b5;

void parse(char * str, int * a, int * b, int j) {

  int i;
  int auxn;
  int flag = 0;
  int flag_neg1 = 0, flag_neg2 = 0;

  for(i=j; i<strlen(str)-1; i++) {

    if((str[i] == ' ') || (str[i] == '-')){
      if (i == j) flag_neg1 = 1;
      else if(str[i] == ' ') flag = 1;
      else flag_neg2 = 1;
    }
    else {
      auxn = str[i] - '0';
      if(flag == 0) {
        *a = *a*10 + auxn;
      } else {
        *b = *b*10 + auxn;
      }
    }
  }

  if(flag_neg1 == 1) *a = *a*-1;
  if(flag_neg2 == 1) *b = *b*-1;

}

void changeActiveChat(TSocket new) {
  pthread_mutex_lock(&socketLock);
    activeSocket = new;
  pthread_mutex_unlock(&socketLock);
}

TSocket checkActiveChat() {
  pthread_mutex_lock(&socketLock);
    TSocket aux = activeSocket;
  pthread_mutex_unlock(&socketLock);
  return aux;
}

void sumContThread(int n) {
  pthread_mutex_lock(&lock);
    if(n > 0) cont_thread += n;
    else cont_thread -= n;
  pthread_mutex_unlock(&lock);
}

void consume(Buffer** b) {
  for(int i=0; i < (*b)->pos; i++) {
    printf("%s", (*b)->buff[i]);
  }
  (*b)->pos = 0;
}

/* Handle client request */
void * HandleRequest(void *args) {
  char str[100];
  TSocket cliSock;
  Buffer* b = (struct Buffer*) malloc(sizeof(Buffer));
  b->pos = 0;

  /* Extract socket file descriptor and id from argument */
  cliSock = ((struct TArgs *) args) -> cliSock;
  free(args);  /* deallocate memory for argument */

  sumContThread(1);

  for(;;) {
    /* Receive the request */
    if (ReadLine(cliSock, str, 99) < 0)
      { ExitWithError("ReadLine() failed");
    } else {
      b->buff[b->pos++] = str;
    }

    char resposta[100];
    /* Operation Types */
    if(cliSock == checkActiveChat()) {
      consume(&b);
      scanf("%s", resposta);
    }
    int resp;
    if (strncmp(str, "quit", 4) == 0) break;
    if (strncmp(str, "threads", 7) == 0) resp = cont_thread;

    sprintf(str, "%s\n", resposta);
    /* Send the response */
    if (WriteN(cliSock, str, strlen(str)) <= 0)
      { ExitWithError("WriteN() failed"); }
  }

  sumContThread(-1);

  close(cliSock);
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  TSocket srvSock, cliSock;        /* server and client sockets */
  struct TArgs *args;              /* argument structure for thread */
  pthread_t threads[NTHREADS];
  int tid = 0;

  if (argc == 1) { ExitWithError("Usage: server <local port>"); }
  if (pthread_mutex_init(&lock, NULL) != 0) {
    ExitWithError("\n mutex init failed\n");
  }

  /* Create a passive-mode listener endpoint */
  srvSock = CreateServer(atoi(argv[1]));

  /* Connect with server that has chat's IP's */
  // printf("Chat server...\n");
  // char* chatServer;
  // scanf("%s", chatServer);
  // TSocket sock = ConnectToServer(chatServer, 2018);

  /* Run forever */
  int id = 1;
  for (;;) {
    if (tid == NTHREADS)
      { ExitWithError("number of threads is over"); }

    /* Spawn off separate thread for each client */
    cliSock = AcceptConnection(srvSock);

    if(activeSocket == 0) changeActiveChat(cliSock);

    /* Create separate memory for client argument */
    if ((args = (struct TArgs *) malloc(sizeof(struct TArgs))) == NULL)
      { ExitWithError("malloc() failed"); }
    args->cliSock = cliSock;
    args->id = id++;
    /* Create a new thread to handle the client requests */
    if (pthread_create(&threads[tid++], NULL, HandleRequest, (void *) args)) {
      { ExitWithError("pthread_create() failed"); }
    }
    /* NOT REACHED */
  }
}
