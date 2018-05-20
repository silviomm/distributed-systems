// Silvio Mançano de Mattos Junior - 2018/1 - Sistemas Distribuídos UFRJ 
#include "mysocket.h"

/* Structure of arguments to pass to client thread */
struct TArgs {
  TSocket cliSock;   /* socket descriptor for client */
};

char nome[50];

void cadastraUsuario(TSocket srvSock);
void descadastraUsuario(TSocket srvSock);
void publish(TSocket srvSock);

int main(int argc, char *argv[]) {
  TSocket srvSock;        /* server socket */

  /* Conecta com Despachante */
  srvSock = ConnectToServer("127.0.0.1", 8000);

  cadastraUsuario(srvSock);
  publish(srvSock);

  close(srvSock);
  return 0;
}

void publish(TSocket srvSock) {
  char msg[140];
  char aux[200];

  for(;;) {
    scanf("%s", msg);
    if(strncmp(msg, "FIM", 3) == 0){
      descadastraUsuario(srvSock);
      break;
    } 
    else {

      //char nome[50] variavel global
      sprintf(aux, "4 %s %s \n", nome, msg);
    
      WriteN(srvSock, aux, strlen(aux));
      ReadLine(srvSock, aux, 99);

      int cod = atoi(aux);
      if(cod == 1) printf("msg publicada!\n");
      else printf("erro ao publicar msg!\n");

    }
  }
}

void cadastraUsuario(TSocket srvSock) {
  char aux[100];

  //char nome[50] variavel global
  printf("nome de usuario...\n");
  scanf("%s", nome);

  sprintf(aux, "1 %s\n", nome);
  WriteN(srvSock, aux, strlen(aux));

  ReadLine(srvSock, aux, 99);
  printf("Resposta Servidor: %s\n", aux);

  int cod = atoi(aux);
  if(cod == 1) printf("usuário registrado!\n");
  else {
    printf("erro: login existente. Tente outro nome\n");
    cadastraUsuario(srvSock);
  }
}

void descadastraUsuario(TSocket srvSock) {
  char aux[100];

  sprintf(aux, "5 %s\n", nome);
  WriteN(srvSock, aux, strlen(aux));

  ReadLine(srvSock, aux, 99);
  int cod = atoi(aux);

  if(cod == 1) printf("usuário descadastrado!\n");
  else printf("erro ao descadastrar usuário!\n");
}
