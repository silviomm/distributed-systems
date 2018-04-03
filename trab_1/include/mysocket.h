// mysocket.h
// October, 2006
// Prof.: Silvana Rossetto 

#include <stdio.h>      /*  printf(), perror() and fprintf() */
#include <sys/socket.h> /*  socket(), bind(), and connect() */
#include <sys/types.h>  /*  predefined symbolic constants and data structures */
#include <arpa/inet.h>  /*  sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /*  atoi() and exit() */
#include <string.h>     /*  memset() */
#include <unistd.h>     /*  close() */
#include <netdb.h>      /*  gethostbyname() */

#define MAXPENDING 10   /* maximum outstanding connection requests */
#define SIZEHOSTNAME 32 /* size of host name */
#define MAXLINELEN 10   /* maximum line size */

/* Socket handle */
typedef int TSocket;

/* Error handling function */
void ExitWithError(char *errorMsg);

/* Error handling function */
void WriteError(char *errorMsg);

/* User message handling function */
void dbg(unsigned short type, char *msg, char *str, int value);

/* Create a server socket */
TSocket CreateServer(unsigned short port); 

/* Accept connection */
TSocket AcceptConnection(TSocket servSock);

/* Connect to a server */
TSocket ConnectToServer(char *servIP, unsigned short servPort);

/* Write "n" bytes to a stream socket */
int WriteN(TSocket sock, char *ptr, int numBytes);

/* Read "n" bytes from a stream socket */
int ReadN(TSocket sock, char *ptr, int numBytes); 

/* Read a line from a socket */
int ReadLine(TSocket sock, char *ptr, int maxLen);

/* Get local name */
char *GetLocalName();  
