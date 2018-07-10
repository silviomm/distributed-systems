 /* 
 * Algorithm FairP2P 
 */

#include "fairp2p.h"

//ports to receive and send messages
int sendPort;
int recvPort;

// wrapper for perror
void error(char *msg) {
  perror(msg);
  exit(1);
}

// Request FairP2P event: send
int fp2pSend (char *dest, char *msg) { 
    int sock; /* socket */
    socklen_t serverlen; /* byte size of server's address */
    struct sockaddr_in serveraddr; /* server's addr */
    struct hostent *server;  /* server host info */
    int ret; /* function return value */

    /* socket: create the socket */
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
        error("error socket(), fp2pSend()");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(dest);
    if (server == NULL) 
        error("error gethostbyname()");

    /* build the server's Internet address */
    memset((void *) &serveraddr, 0, sizeof(serveraddr)); 
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(sendPort);

    /* send the message to the server */
    serverlen = sizeof(serveraddr);
    ret = sendto(sock, msg, strlen(msg), 0, (struct sockaddr *) &serveraddr, serverlen);

    /* close the socket */
    close(sock);
    return ret;
}

// receive a message
int receive (char *src, char *msg) {
   int sock; /* socket */
   socklen_t clientlen; /* byte size of client's address */
   struct sockaddr_in serveraddr; /* server's addr */
   struct sockaddr_in clientaddr; /* client addr */
   //struct hostent *hostp; /* client host info */
   int optval; /* flag value for setsockopt */
   int n; /* message byte size */
   char *hostaddrp; /* dotted decimal host addr string */

   /* socket: create the socket */
   sock = socket(AF_INET, SOCK_DGRAM, 0);
   if (sock < 0)
     error("error socket(), delivery()");

   /* eliminates "ERROR on binding: address already in use" error. */
   optval = 1;
   setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
             (const void *)&optval , sizeof(int));

   /* build the server's Internet address */
   memset((void *) &serveraddr, 0, sizeof(serveraddr));
   serveraddr.sin_family = AF_INET;
   serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
   serveraddr.sin_port = htons((unsigned short) recvPort);

   /* bind: associate the parent socket with a port */
   if (bind(sock, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
       error("erro bind()");

   /* recvfrom: receive a UDP datagram from a client */
   clientlen = sizeof(clientaddr);
   memset(msg, 0, BUFSIZE);
   n = recvfrom(sock, msg, BUFSIZE, 0, (struct sockaddr *) &clientaddr, &clientlen);
   if (n < 0) return n;

   /* get the source host address */
   memset(src, 0, BUFSIZE);
   hostaddrp = inet_ntoa(clientaddr.sin_addr);
   if (hostaddrp == NULL)
     error("error inet_ntoa()");
   strcpy(src, hostaddrp);
   
   /* close the socket */
   close(sock);

   /* return  the  number  of bytes received, or -1 if an error occurred */
   return n;
}

// Indication FairP2P event: delivery
// Internal thread to wait for messages from other nodes
void * mythread(void * arg) {
  char msg[BUFSIZE]; /* message buf */
  char src[BUFSIZE]; /* source node */
  printf("Thread to receive messages is run...\n");
  while(1) {
      receive(src, msg);
      fp2pDelivery(src, msg);
  }
  pthread_exit(NULL);
}

// FairP2P init event
pthread_t fp2pInit(int fp2psendPort, int fp2precvPort) {
    sendPort = fp2psendPort; //port to send messages to dst nodes
    recvPort = fp2precvPort; //port to receive messages from src nodes
 
    /* create a internal thread to receive and delivery messages */
    pthread_t tid;
    if(pthread_create(&tid, NULL, mythread, NULL))
       error("error pthread_create()");
    return tid;
}
