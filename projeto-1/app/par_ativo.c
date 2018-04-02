#include "mysocket.h"

int sendMsg(char* str, TSocket sock) {
   scanf(" %[^\n]",str);
   int n = strlen(str);
   str[n] = '\n';

   if (WriteN(sock, str, ++n) <= 0)
     { ExitWithError("WriteN() failed"); }
   
   if (strncmp(str, "FIM", 3) == 0) return -1;
   if (strncmp(str, "LISTEN", 6) == 0) return -2;
   
   
   return 1;
}

void receiveMsg(char* str, TSocket sock) {
   if (ReadLine(sock, str, 99) < 0)
    { ExitWithError("ReadLine() failed");
   } else printf("%s",str);
}

int main(int argc, char *argv[]) {
  TSocket sock;
  char *servIP;
  unsigned short servPort;
  char str[100];

  if (argc != 3) {
    ExitWithError("Usage: client <remote server IP> <remote server Port>");    
  }

  servIP = argv[1];
  servPort = atoi(argv[2]);

  /* Create a connection */
  sock = ConnectToServer(servIP, servPort);

  int option = 1;
  for(;;) {
    while(option == 1) {
      option = sendMsg(str, sock);
    }
    if(option == -1) {
      break;
    }
    if(option == -2) {
      receiveMsg(str, sock);
    }
  }

  close(sock);
  return 0;
}
