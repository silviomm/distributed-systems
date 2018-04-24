#include "mysocket.h"  

void ve_quem_ta_online(char* ip) {  
  printf("Pegando lista de usuários...\n");
  TSocket sock = ConnectToServer(ip, 2018); //2018 porta padrão do projeto
  
  char str[5] = "2\n";
  if (WriteN(sock, str, strlen(str)) <= 0) { 
    ExitWithError("WriteN() desconecta_servidor_usuarios failed"); 
  }
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    ExitWithError("Usage: ./client <ip users server>");    
  }

  ve_quem_ta_online(argv[1]);

  return 0;
}
