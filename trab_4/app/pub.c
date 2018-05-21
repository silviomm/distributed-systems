// Silvio Mançano de Mattos Junior - 2018/1 - Sistemas Distribuídos UFRJ 
#include "mysocket.h"

#define SERVER "127.0.0.1"
#define PORT 8000

/* Structure of arguments to pass to client thread */
struct TArgs {
  TSocket cliSock;   /* socket descriptor for client */
};

char nome[50];

void cadastraUsuario();
void descadastraUsuario();
void publish();

int main(int argc, char *argv[]) {
  printf("versao 1.0\n");

  cadastraUsuario();
  printf("publique mensagens!\n");
  publish();

  return 0;
}

void publish() {
  char msg[140];
  char aux[200];

  for(;;) {
    fgets(msg, 140, stdin);
    if(strncmp(msg, "FIM", 3) == 0){
      descadastraUsuario();
      break;
    } 
    else {

      TSocket srvSock = ConnectToServer(SERVER, PORT);

      //char nome[50] variavel global
      sprintf(aux, "4 %s %s \n", nome, msg);
    
      WriteN(srvSock, aux, strlen(aux));
      ReadLine(srvSock, aux, 99);

      int cod = atoi(aux);
      if(cod == 1) printf("msg publicada!\n");
      else printf("erro ao publicar msg!\n");

      close(srvSock);

    }
  }
}

void cadastraUsuario() {
  char aux[100];

  TSocket srvSock = ConnectToServer(SERVER, PORT);

  //char nome[50] variavel global
  printf("nome de usuario...\n");
  scanf("%s", nome);

  sprintf(aux, "1 %s \n", nome);
  WriteN(srvSock, aux, strlen(aux));

  ReadLine(srvSock, aux, 99);
  close(srvSock);

  printf("Resposta Servidor: %s\n", aux);

  int cod = atoi(aux);
  if(cod == 1) printf("usuário registrado!\n");
  else {
    printf("erro: login existente. Tente outro nome\n");
    cadastraUsuario();
  }

}

void descadastraUsuario() {
  char aux[100];
  
  TSocket srvSock = ConnectToServer(SERVER, PORT);

  sprintf(aux, "5 %s \n", nome);
  WriteN(srvSock, aux, strlen(aux));

  ReadLine(srvSock, aux, 99);
  int cod = atoi(aux);

  if(cod == 1) printf("usuário descadastrado!\n");
  else printf("erro ao descadastrar usuário!\n");

  close(srvSock);
}
