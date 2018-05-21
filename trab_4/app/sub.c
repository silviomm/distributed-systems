// Silvio Mançano de Mattos Junior - 2018/1 - Sistemas Distribuídos UFRJ 
#include "mysocket.h"

#define QNTD_MAX_SUB 100

struct inscricao {
  char nome[50];
  int id;
  int ativo;
};
struct inscricao inscricoes[QNTD_MAX_SUB];

int QNTD_ATUAL_SUB = 0;

void subscribe(TSocket despachante, int portaLocal);
void receiveMsg(TSocket sock);
void unsubscribe(TSocket despachante);
void unsubscribeAll(TSocket despachante);

int main(int argc, char *argv[]) {
  TSocket despachante;
  char str[100];
  fd_set set; int ret;
  int portaLocal; char* servIP; unsigned short servPort;

  if (argc != 4) {
    ExitWithError("Usage: ./sub <portaServidorLocal> <ipDespachante> <portaDespachante>");
  }
  else {
    portaLocal = atoi(argv[1]);
    servIP = argv[2];
    servPort = atoi(argv[3]);
  }

  /* Cria servidor local */
  CreateServer(portaLocal);

  /* Conecta com despachante e se inscreve em 1 ou mais publishers */
  despachante = ConnectToServer(servIP, servPort);
  subscribe(despachante, portaLocal);

  for(;;) {
    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);
    FD_SET(despachante, &set);

    printf("Digite 1 para se inscrever em novos canais\n 2 para se desinscrever de um ou mais canais\n FIM para sair do app\n ou espere uma msg...");

    ret = select (FD_SETSIZE, &set, NULL, NULL, NULL);
    if (ret<0) {
       WriteError("select() failed");
       break;
    }

    /* Read from stdin */
    if (FD_ISSET(STDIN_FILENO, &set)) {
      scanf("%s", str);
      if(strncmp(str, "1", 1) == 0) subscribe(despachante, portaLocal);
      if(strncmp(str, "2", 1) == 0) unsubscribe(despachante);
      if(strncmp(str, "FIM", 3) == 0) { unsubscribeAll(despachante); break; }
    }

    /* Read from srvSock */
    if (FD_ISSET(despachante, &set)) {
      receiveMsg(despachante);
    }

  }

  close(despachante);
  return 0;
}

void receiveMsg(TSocket sock) {
  char str[100];
  if (ReadLine(sock, str, 99) < 0)
    ExitWithError("ReadLine() failed");
  else {
    printf("%s\n",str);
  }
}

void subscribe(TSocket despachante, int portaLocal) {
  char canal[50];
  char aux[100];

  printf("digite o canal que deseja seguir ou FIM para sair...\n");
  scanf("%s", canal);

  while(strncmp(canal, "FIM", 3) != 0) {

    sprintf(aux, "2 %s %d \n", canal, portaLocal);
    WriteN(despachante, aux, strlen(aux));

    ReadLine(despachante, aux, 99);
    int cod = atoi(aux);

    if(cod == 0) printf("erro ao se inscrever nesse canal\n");
    else {
      //como verificar "<0> <codUnique>" ?
      //inscricoes[QNTD_ATUAL_SUB].id = cod;
      sprintf(inscricoes[QNTD_ATUAL_SUB].nome, "%s", canal);
      inscricoes[QNTD_ATUAL_SUB].ativo = 1;
      QNTD_ATUAL_SUB++;
    }

    printf("digite o canal que deseja seguir ou FIM para sair...\n");
    scanf("%s", canal);
    
  }
}

void unsubscribe(TSocket despachante) {
  char aux[50];
  do {
    printf("De qual canal deseja de desinscrever?\n");
    for(int i=0; i < QNTD_ATUAL_SUB; i++) {
      if(inscricoes[i].ativo != 0) printf("Canal %d: %s\n", inscricoes[i].id, inscricoes[i].nome);
    }
    printf("Ou digite FIM para sair...");

    scanf("%s", aux);
    if(strncmp(aux, "FIM", 3) != 0) {
      int cod = atoi(aux);
      char req[100];
      sprintf(req, "3 %d \n", cod);

      WriteN(despachante, req, strlen(req));
      ReadLine(despachante, req, 99);

      cod = atoi(req);
      if(cod == 1){
        int canalDesinscrito = atoi(aux);
        for(int i=0; i < QNTD_ATUAL_SUB; i++) {
          if(inscricoes[i].id == canalDesinscrito) inscricoes[i].ativo = 0;
        }  
        printf("desinscricao com sucesso!\n");
      } 
      else printf("codigo de desinscricao nao existe!\n");
    }

  } while(strncmp(aux, "FIM", 3) == 0);
}

void unsubscribeAll(TSocket despachante) {
  char aux[50];
  for(int i=0; i < QNTD_ATUAL_SUB; i++) {
    sprintf(aux, "3 %d \n", inscricoes[i].id);

    WriteN(despachante, aux, strlen(aux));
    ReadLine(despachante, aux, 99);

    int cod = atoi(aux);
    if(cod == 1) printf("desinscricao %d com sucesso!\n", inscricoes[i].id);
    else printf("codigo de desinscricao nao existe!\n");
  }
}


