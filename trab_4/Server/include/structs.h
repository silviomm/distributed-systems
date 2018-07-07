#include <stdio.h>      /*  printf(), perror() and fprintf() */
#include <sys/socket.h> /*  socket(), bind(), and connect() */
#include <sys/types.h>  /*  predefined symbolic constants and data structures */
#include <arpa/inet.h>  /*  sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /*  atoi() and exit() */
#include <string.h>     /*  memset() */
#include <unistd.h>     /*  close() */
#include <netdb.h>      /*  gethostbyname() */
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <iostream>
#include <set>
#include "mysocket.h"
#define pb push_back
#define BUFSIZE 100
#define NUMTHREADS 100
#define IPSIZE 16
using namespace std;
struct Message{
	std::vector< std:: string> lista;
};


struct TBuffer{
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Quem estiver escrevendo ou consumindo ira blockear o uso pela thread
  queue<string> messages;
};
/* Structure of arguments to pass to client thread */
struct TArgs {
  TSocket cliSock;   /* socket descriptor for client */
  int IP,PORT;          // Variavel que ira armazenar quem acessar
  bool operator < (const TArgs &a) const { return IP < a.IP;}
};
