/* Application to experiment the FairP2P module */

#include "fairp2p.h"

#define SENDPORT 4000
#define RECVPORT 3000

// !!! Must be implemented to handle the indication event !!!
void fp2pDelivery (char *src, char *msg) {
  printf("Recebi: %s %s\n", src, msg);
  fp2pSend(src, msg);
}   

// main function
int main(int argc, char **argv) {
  pthread_t tid;
  tid = fp2pInit(SENDPORT, RECVPORT);
  pthread_join(tid, NULL);
}
