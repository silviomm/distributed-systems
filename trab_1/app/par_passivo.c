#include "mysocket.h"
#include <pthread.h>

#define NTHREADS 5
#define TAM_BUFFER 100

int cont_thread = 0;
pthread_mutex_t lock, socketLock;
TSocket activeSocket = 0;

/* Structure of arguments to pass to client thread */
struct TArgs {
  TSocket cliSock;   /* socket descriptor for client */
  int id;
};

typedef struct Buffer {
  char* buff[TAM_BUFFER];
  int pos;
} Buffer;

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

void produce(char* str, Buffer** b) {
  if ((*b)->pos >= TAM_BUFFER) (*b)->pos = 0;
  (*b)->buff[(*b)->pos++] = str;
}

/* Handle client request */
void * HandleRequest(void *args) {
  char str[100];
  TSocket cliSock;
  char response[100];

  /* Create Buffer and set initial position */
  Buffer* b = (struct Buffer*) malloc(sizeof(Buffer));
  b->pos = 0;

  /* Extract socket file descriptor and deallocate memory from argument */
  cliSock = ((struct TArgs *) args) -> cliSock;
  free(args);

  sumContThread(1);

  for(;;) {
    /* Receive the request */
    if (ReadLine(cliSock, str, 99) < 0) { 
      ExitWithError("ReadLine() failed");
    } 
    else {
      produce(str, &b);
    }

    /* Consume msg and send response */
    if(cliSock == checkActiveChat()) {
      consume(&b);
      if (strncmp(str, "FIM", 3) == 0) break; /* finish chat */
      
      scanf("%s", response);
      
      /* change conversation or send response */
      if (strncmp(response, "/change", 7) == 0) {
        changeActiveChat(1);
      }
      else {
        sprintf(str, "%s\n", response);
        if (WriteN(cliSock, str, strlen(str)) <= 0) { 
          ExitWithError("WriteN() failed"); 
        }
      }
    }
  }

  sumContThread(-1);

  close(cliSock);
  pthread_exit(NULL);
}


void * Menu(void *args) {
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
