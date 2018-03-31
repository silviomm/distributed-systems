// mysocket.c
// October, 2006
// Prof.: Silvana Rossetto 

#include "mysocket.h"

/* Error handling */
void ExitWithError(char *errorMsg) {
  printf("-- EXIT: %s\n", errorMsg);
  exit(1);
}

/* Write an error message */
void WriteError(char *errorMsg) {
  printf("-- ERROR: %s\n", errorMsg);
}

/* Write debug */
void dbg(unsigned short type, char *msg, char *str, int value) {
  switch (type) {
    case 1: printf("%s{} \n", msg); break;
    case 2: printf("%s{str=\"%s\"} \n", msg, str); break;
    case 3: printf("%s{value=%d} \n", msg, value); break;
    case 4: printf("%s{str=\"%s\",value=%d} \n", msg, str, value); break;
    default: printf("-- %s\n", msg);
  }
}

/* Create a server connection */
TSocket CreateServer(unsigned short port) {
  TSocket sock;                        /* socket to create */
  struct sockaddr_in addr;             /* local address */

  /* Create a TCP local endpoint */
  if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    { ExitWithError("socket() failed"); }

  /* Setup the local address */
  memset((void *) &addr, 0, sizeof(addr));       /* zero out structure */
  addr.sin_family = AF_INET;                     /* Internet address family */
  addr.sin_addr.s_addr = htonl(INADDR_ANY);      /* any incoming interface */
  addr.sin_port = htons(port);                   /* local port */

  /* Bind to the local address */
  if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0)
    { ExitWithError("bind() failed"); }

  /* Mark the socket so it will listen for incoming connections */
  if (listen(sock, MAXPENDING) < 0)
    { ExitWithError("listen() failed"); }
  return sock;
}

/* Connection accepting */
TSocket AcceptConnection(TSocket srvSock) {
  TSocket cliSock;                 /* socket descriptor for client */
  struct sockaddr_in cliAddr;      /* client address */   
  unsigned int cliLen;             /* length of client address data structure */

  /* Set the size of the in-out parameter */
  cliLen = sizeof(cliAddr);

  /* Wait for a client to connect */
  if ((cliSock = accept(srvSock, (struct sockaddr *) &cliAddr, &cliLen)) < 0)
    { ExitWithError("accept() failed"); }

  /* cliSock is connected to a client! */
  return cliSock;
}

/* Connect to a server */
TSocket ConnectToServer(char *servIP, unsigned short servPort) {
  TSocket sock;                    /* socket to create */
  struct sockaddr_in addr;         /* local address */

  /* Create a TCP local endpoint */
  if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) 
   { ExitWithError("socket() failed"); }

  /* Setup the address of server */
  memset(&addr, 0, sizeof(addr));           /* zero out structure */
  addr.sin_family = AF_INET;                /* Internet address family */
  addr.sin_addr.s_addr = inet_addr(servIP); /* server IP address */
  addr.sin_port = htons(servPort);          /* server port */

  /* Create a connection with remote server */
  if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0)
    { ExitWithError("connect() failed"); }

  return sock;
}

/* Write "n" bytes to a stream socket */
int WriteN(TSocket sock, char *ptr, int numBytes) {
  int numLeft, numWritten;

  numLeft = numBytes;
  while (numLeft > 0) {
    numWritten = write(sock, ptr, numLeft);
    if (numWritten <= 0) return(numWritten);  /* error */
    numLeft -= numWritten;
    ptr += numWritten;
  }
  return (numBytes - numLeft);
}

/* Read "n" bytes from a stream socket */
int ReadN(TSocket sock, char *ptr, int numBytes) {
  int numLeft, numRead;

  numLeft = numBytes;
  while (numLeft > 0) {
    numRead = read(sock, ptr, numLeft);
    if (numRead < 0) return (numRead);    /* error */
    else if (numRead == 0) break;         /* EOF */
    numLeft -= numRead;
    ptr += numRead;
  }
  return (numBytes - numLeft);            /* return >= 0 */
}

/* Read a line from a stream socket */
int ReadLine(TSocket sock, char *ptr, int maxLen) {
  int n, rc; char c;
  
  for (n = 1; n < maxLen; n++) {
    if ( (rc = read(sock, &c, 1)) == 1) {
      *ptr++ = c;
      if (c == '\n') break;
    } else if (rc == 0) {
      if (n == 1) return(0);   /* EOF, no data read */
      else break;              /* EOF, some data was read */
    } else return(-1);         /* error */
  }
  *ptr = '\0';
  return(n);
}
