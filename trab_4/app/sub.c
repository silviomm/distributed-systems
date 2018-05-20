// Silvio Mançano de Mattos Junior - 2018/1 - Sistemas Distribuídos UFRJ 
#include "mysocket.h"  

int main(int argc, char *argv[]) {
  TSocket sock;
  char *servIP;                /* server IP */
  unsigned short servPort;     /* server port */
  char str[100];
  int n;

  if (argc != 3) {
    ExitWithError("Usage: client <remote server IP> <remote server Port>");    
  }

  servIP = argv[1];
  servPort = atoi(argv[2]);

  /* Create a connection */
  sock = ConnectToServer(servIP, servPort);

  /* Write msg */
  for(;;) {
    scanf(" %[^\n]",str);
    n = strlen(str);
    str[n] = '\n';
    if (WriteN(sock, str, ++n) <= 0)
      { ExitWithError("WriteN() failed"); }

    /* Receive the response */
    if (ReadLine(sock, str, 99) < 0)
    { ExitWithError("ReadLine() failed");} 
    else printf("%s",str);
  }
  close(sock);
  return 0;
}
