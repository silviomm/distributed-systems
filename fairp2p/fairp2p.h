/* 
 * Module FairP2P 
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>

//Max buffer size for messages and sources
#define BUFSIZE 1024

// wrapper for perror
void error(char *msg);

// FairP2P init event
pthread_t fp2pInit(int fp2psendPort, int fp2precvPort);

// Request FairP2P event: send
int fp2pSend (char *dest, char *msg); 

// Indication FairP2P event: delivery
void fp2pDelivery(char *src, char *msg);
