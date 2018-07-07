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
int portaLocal; char* servIP; unsigned short servPort;

void subscribe(int portaLocal);
void receiveMsg(TSocket sock);
void unsubscribe();
void unsubscribeAll();

int main(int argc, char *argv[]) {
  TSocket srvLocal;
  char str[100];
  fd_set set; int ret;

  if (argc != 4) {
    ExitWithError("Usage: ./sub <portaServidorLocal> <ipDespachante> <portaDespachante>");
  }
  else {
    portaLocal = atoi(argv[1]);
    servIP = argv[2];
    servPort = atoi(argv[3]);
  }

  /* Cria servidor local */
  srvLocal = CreateServer(portaLocal);

  /* Conecta com despachante e se inscreve em 1 ou mais publishers */
  subscribe(portaLocal);

  for(;;) {
    setbuf(stdin, NULL);
    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);
    FD_SET(srvLocal, &set);

    printf(" Digite 1 para se inscrever em novos canais\n"); 
    printf(" Digite 2 para se desinscrever de um ou mais canais\n");
    printf(" Digite FIM para sair do app\n");
    printf(" Ou espere uma msg...\n");

    ret = select (FD_SETSIZE, &set, NULL, NULL, NULL);
    if (ret<0) {
       WriteError("select() failed");
       break;
    }

    /* Read from stdin */
    if (FD_ISSET(STDIN_FILENO, &set)) {
      scanf("%s", str);
      if(strncmp(str, "1", 1) == 0) subscribe(portaLocal);
      if(strncmp(str, "2", 1) == 0) unsubscribe();
      if(strncmp(str, "FIM", 3) == 0) { unsubscribeAll(); break; }
    }

    /* Read from srvSock */
    if (FD_ISSET(srvLocal, &set)) {
      TSocket cliSock = AcceptConnection(srvLocal);
      receiveMsg(cliSock);
    }

  }

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

void subscribe(int portaLocal) {
  char canal[50];
  char aux[100];
  TSocket despachante;

  printf("digite o canal que deseja seguir ou FIM para sair...\n");
  scanf("%s", canal);

  while(strncmp(canal, "FIM", 3) != 0) {

    despachante = ConnectToServer(servIP, servPort);

    sprintf(aux, "2 %s %d \n", canal, portaLocal);
    WriteN(despachante, aux, strlen(aux));

    ReadLine(despachante, aux, 99);

    char * ptr; int cod = 0, cod1;
    ptr = strtok(aux, " ");
    while(ptr!=NULL) {
      cod1 = cod;
      cod = atoi(ptr);
      ptr = strtok(NULL, " ");
    }


    if(cod1 == 0) printf("erro ao se inscrever nesse canal\n");
    else {
      printf("inscrição bem sucedida!\n");
      printf("cod de inscrição: %d\n", cod1);
      inscricoes[QNTD_ATUAL_SUB].id = cod1;
      sprintf(inscricoes[QNTD_ATUAL_SUB].nome, "%s", canal);
      inscricoes[QNTD_ATUAL_SUB].ativo = 1;
      QNTD_ATUAL_SUB++;
    }

    close(despachante);

    printf("digite o canal que deseja seguir ou FIM para sair...\n");
    scanf("%s", canal);
    
  }
}

void unsubscribe() {
  char aux[50];
  do {
    printf("De qual canal deseja de desinscrever?\n");
    for(int i=0; i < QNTD_ATUAL_SUB; i++) {
      if(inscricoes[i].ativo != 0) printf("Canal %d: %s\n", inscricoes[i].id, inscricoes[i].nome);
    }
    printf("Ou digite FIM para sair...\n");

    scanf("%s", aux);
    if(strncmp(aux, "FIM", 3) != 0) {
      TSocket despachante = ConnectToServer(servIP, servPort);
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

  } while(strncmp(aux, "FIM", 3) != 0);
}

void unsubscribeAll() {
  char aux[50];

  for(int i=0; i < QNTD_ATUAL_SUB; i++) {
    if(inscricoes[i].ativo != 0) {
      TSocket despachante = ConnectToServer(servIP, servPort);
      sprintf(aux, "3 %d \n", inscricoes[i].id);

      WriteN(despachante, aux, strlen(aux));
      ReadLine(despachante, aux, 99);

      int cod = atoi(aux);
      if(cod == 1) printf("desinscricao do Canal: %d %s com sucesso!\n", inscricoes[i].id, inscricoes[i].nome);
      else printf("codigo de desinscricao nao existe!\n");
    }
  }
}


