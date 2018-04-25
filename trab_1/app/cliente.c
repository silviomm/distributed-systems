#include "mysocket.h"  

void ve_quem_ta_online(char* ip) {  
  printf("Pegando lista de usuários...\n");
  TSocket sock = ConnectToServer(ip, 2018); //2018 porta padrão do projeto
  
  /* envia comando de listagem */
  char str[100] = "2\n";
  if (WriteN(sock, str, strlen(str)) <= 0) { 
    ExitWithError("WriteN() desconecta_servidor_usuarios failed"); 
  }

  /* recebe quantidade e lista de usuarios*/
  if (ReadLine(sock, str, 99) < 0)
    ExitWithError("ReadLine() failed");
  else 
    printf("\nExistem %s usuarios online...\n",str);
  int qnt;
  sscanf(str, "%d", &num);
  for (int i=0; i < num; i++) {
    if (ReadLine(sock, str, 99) < 0)
      ExitWithError("ReadLine() failed");
    else 
      printf("%s",str);    
  }
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    ExitWithError("Usage: ./client <ip users server>");    
  }

  ve_quem_ta_online(argv[1]);

  return 0;
}
