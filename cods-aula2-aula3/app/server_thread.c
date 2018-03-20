// A multithread echo server

#include "mysocket.h"
#include <pthread.h>

#define NTHREADS 100

int cont_thread = 0;
pthread_mutex_t lock;

/* Structure of arguments to pass to client thread */
struct TArgs {
  TSocket cliSock;   /* socket descriptor for client */
};

int sum(int a, int b) {
  return a+b;
}
int sub(int a, int b) {
  return a-b;
}
int mult(int a, int b) {
  return a*b;
}

//problemas: div por 0 ; div sera arredondada
int divS(int a, int b) {
  return a/b;
}

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

/* Handle client request */
void * HandleRequest(void *args) {
  char str[100];
  TSocket cliSock;

  /* Extract socket file descriptor from argument */
  cliSock = ((struct TArgs *) args) -> cliSock;
  free(args);  /* deallocate memory for argument */

  pthread_mutex_lock(&lock);
  cont_thread++;
  pthread_mutex_unlock(&lock);

  for(;;) {
    /* Receive the request */
    if (ReadLine(cliSock, str, 99) < 0)
      { ExitWithError("ReadLine() failed");
    } else printf("%s",str);

    /* Operation Types */
    int resp;
    if (strncmp(str, "quit", 4) == 0) break;
    if (strncmp(str, "threads", 7) == 0) resp = cont_thread;

    int a = 0, b = 0;
    if (strncmp(str, "sum", 3) == 0){
      parse(str, &a, &b, 4);
      resp = sum(a, b);
    }
    if (strncmp(str, "sub", 3) == 0) {
      parse(str, &a, &b, 4);
      resp = sub(a, b);
    }
    if (strncmp(str, "mult", 4) == 0) {
      parse(str, &a, &b, 5);
      resp = mult(a, b); 
    }
    if (strncmp(str, "div", 3) == 0) {
      parse(str, &a, &b, 4);
      resp = divS(a, b);
    }

    sprintf(str, "%d\n", resp);
    /* Send the response */
    if (WriteN(cliSock, str, strlen(str)) <= 0)
      { ExitWithError("WriteN() failed"); }
  }

  pthread_mutex_lock(&lock);
  cont_thread--;
  pthread_mutex_unlock(&lock);

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

  /* Run forever */
  for (;;) {
    if (tid == NTHREADS)
      { ExitWithError("number of threads is over"); }

    /* Spawn off separate thread for each client */
    cliSock = AcceptConnection(srvSock);

    /* Create separate memory for client argument */
    if ((args = (struct TArgs *) malloc(sizeof(struct TArgs))) == NULL)
      { ExitWithError("malloc() failed"); }
    args->cliSock = cliSock;

    /* Create a new thread to handle the client requests */
    if (pthread_create(&threads[tid++], NULL, HandleRequest, (void *) args)) {
      { ExitWithError("pthread_create() failed"); }
    }
    /* NOT REACHED */
  }
}
