#include "mysocket.h"

int sendMsg(char* str, TSocket sock) {
   scanf(" %[^\n]",str);
   int n = strlen(str);
   str[n] = '\n';

   if (WriteN(sock, str, ++n) <= 0)
     { ExitWithError("WriteN() failed"); }

   if (strncmp(str, "FIM", 3) == 0) return -1;

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
  fd_set set;
  int ret;

  if (argc != 3) {
    ExitWithError("Usage: client <remote server IP> <remote server Port>");
  }

  servIP = argv[1];
  servPort = atoi(argv[2]);

  /* Create a connection */
  sock = ConnectToServer(servIP, servPort);

  for(;;) {
    /* Initialize the file descriptor set */
    FD_ZERO(&set);
    /* Include stdin into the file descriptor set */
    FD_SET(STDIN_FILENO, &set);
    /* Include srvSock into the file descriptor set */
    FD_SET(sock, &set);

    ret = select (FD_SETSIZE, &set, NULL, NULL, NULL);
    if (ret<0) {
       WriteError("select() failed");
       break;
    }

    /* Read from stdin */
    if (FD_ISSET(STDIN_FILENO, &set)) {
      if (sendMsg(str, sock) < 0) break;
    }

    /* Read from srvSock */
    if (FD_ISSET(sock, &set)) {
      receiveMsg(str, sock);
    }

  }

  close(sock);
  return 0;
}
