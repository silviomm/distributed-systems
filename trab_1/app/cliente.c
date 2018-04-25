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
    printf("usuarios online... %s\n",str);
  int qnt;
  sscanf(str, "%d", &qnt);
  for (int i=0; i < qnt; i++) {
    if (ReadLine(sock, str, 99) < 0)
      ExitWithError("ReadLine() failed");
    else 
      printf("%s",str);    
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    ExitWithError("Usage: ./cliente <ip users server>");    
  }

  ve_quem_ta_online(argv[1]);

  return 0;
}
