#include "mysocket.h"
#include <pthread.h>
#define MAX_NR_USERS 20
#define MAX_SIZE_NAME 40
#define NTHREADS 5

void getIP(TSocket cliSock, char *ip) {
  unsigned int cliLen;
  struct sockaddr_in cliAddr;
  memset((void *) &cliAddr, 0, sizeof(cliAddr));
  cliLen = sizeof(cliAddr);

  if (getpeername(cliSock, (struct sockaddr *) &cliAddr, &cliLen)) {
    ExitWithError("Error in getting IP");
  }

  strcpy(ip, inet_ntoa(cliAddr.sin_addr));
}

typedef struct {
  TSocket cliSock;
  char ip[16];
} TArgs;

typedef struct {
  char name[MAX_SIZE_NAME];
  char ip[16];
  char port[7];
} user_t;

pthread_mutex_t mutex_print = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_user_edit = PTHREAD_MUTEX_INITIALIZER;

int users_logged;
int users_registered;
int users_registered_active;
user_t users[MAX_NR_USERS];

int ParseOption(char *str) {
  switch(str[0]) {
    case '1':
	  return 1;
	case '2':
	  return 2;
	case '3':
	  return 3;
	default:
      return -1;
  }
}

int LoginExists (char *login) {
  int i;
  for (i = 0; i < users_registered; i++)
    if (!strcmp(login, users[i].name))
	  return 1;

  return 0;
}

int ParseNameAndPort(char *request, char *out_name, char *out_port){
  char *copy = strdup(request);

  char *tok = copy, *end = copy;
  strsep(&end, " ");
  tok = end;

  strsep(&end, " ");
  if(LoginExists(tok))
    return 1;
  
  if(strlen(tok) > MAX_SIZE_NAME)
    tok[MAX_SIZE_NAME-1] = '\0';
  strcpy(out_name, tok);

  tok = end;
  strsep(&end, "\n ");
  if(strlen(tok))
    strcpy(out_port, tok);

  free(copy);

  return 0;
}

void ResponseUserLoginExists(TSocket cliSock) {
  char error[] = "1\n";
  int err_len = strlen(error);

  if (WriteN(cliSock, error, err_len) <= 0) {
    ExitWithError("WriteN() in error (add, 1) failed");
  }
}

void ResponseAddLimitExceeded(TSocket cliSock) {
  char error[] = "0\n";
  int err_len = strlen(error);

  if (WriteN(cliSock, error, err_len) <= 0) {
    ExitWithError("WriteN() in error (add, 0) failed");
  }
}

void ResponseAddOK(TSocket cliSock) {
  char ok[] = "2\n";
  int ok_len = strlen(ok);

  if (WriteN(cliSock, ok, ok_len) <= 0) {
    ExitWithError("WriteN() in ok (add, 2) failed");
  }
}

void AddUser(TSocket cliSock, char *str, char ip[16]) {
  if (users_registered >= 20) {
    ResponseAddLimitExceeded(cliSock);
    return;
  }

  if (ParseNameAndPort(str, users[users_registered].name, users[users_registered].port)){
    ResponseUserLoginExists(cliSock);
    return;
  }
    
  strcpy(users[users_registered].ip, ip);

  users_registered++;
  users_registered_active++;
  ResponseAddOK(cliSock);
}

void ResponseNrUsers(TSocket cliSock) {
  char responseNrUsers[5];
  snprintf(responseNrUsers, 5, "%i \n", users_registered_active);
  
  if (WriteN(cliSock, responseNrUsers, strlen(responseNrUsers)) <= 0) {
    ExitWithError("WriteN() in ResponseNrUsers failed");
  }
}

void ResponseAllUsers(TSocket cliSock) {
  int i;
  for (i=0; i < users_registered; i++) {
    if (users[i].name[0] == '\0')
	  continue;

    char responseThisUser[70];
	snprintf(responseThisUser, 70, "%s %s %s \n", 
	         users[i].name, users[i].ip, users[i].port);
    int res_len = strlen(responseThisUser);

	if (WriteN(cliSock, responseThisUser, res_len) <= 0)
	  ExitWithError("WriteN() in ResponseAllUsers failed");
  }
}

void SendUserList(TSocket cliSock) {
  ResponseNrUsers(cliSock);
  ResponseAllUsers(cliSock);
}

void RemoveUser(char *str) {
  int i;

  char *copy = strdup(str);
  char *tok = copy, *end = copy;
  strsep(&end, " ");
  tok = end;
  strsep(&end, "\n");

  for (i=0; i<users_registered; i++) {
    if (!strcmp(tok, users[i].name)) {
      users[i].name[0] = '\0';
      users[i].ip[0] = '\0';
      users[i].port[0] = '\0';
      users_registered_active--;
      break;
	  }
  }

  free(copy);
}

void * HandleRequest(void *args) {
  char str[100];
  int user_option;
  TSocket cliSock;

  cliSock = ((TArgs *) args) -> cliSock;

  if (ReadLine(cliSock, str, 99) < 0) {
    WriteError("\nRequest message can't be read");
    return NULL;
  } 
 
  user_option = ParseOption(str);	
  
  switch(user_option) {
    case 1:
      printf("\n+\n+ %s\n+ ADD\n+\n", ((TArgs *) args) -> ip);
      AddUser(cliSock, str, ((TArgs *)args)->ip);
      break;
    case 2:
      printf("\n+\n+ %s\n+ SEND\n+\n", ((TArgs *) args) -> ip);
      SendUserList(cliSock);
      break;
    case 3:
      printf("\n+\n+ %s\n+ REMOVE\n+\n", ((TArgs *) args) -> ip);
      RemoveUser(str);
      break;
    default:
      printf("\nInvalid option in user\n");
      if (WriteN(cliSock, "0\n", 2) <= 0)
        ExitWithError("WriteN() in ResponseAllUsers failed");
  }

//  pthread_mutex_lock( &mutex_print );
//  pthread_mutex_unlock( &mutex_print);


  pthread_mutex_lock( &mutex_print );
  users_logged--;
  printf("\n|Connection Closed\n");
  printf("|-IP: %s\n", ((TArgs *) args) -> ip);
  pthread_mutex_unlock( &mutex_print );

  free(args);
  close(cliSock);
  pthread_exit(NULL);
}


int main(int argc, char **argv) {
  TSocket srvSock, cliSock;
  TArgs *args;
  pthread_t threads[NTHREADS];
  int tid = 0;
  int i;
  fd_set set;
  char stdin_string[60];

  users_logged = 0;
  users_registered = 0;
  users_registered_active = 0;

  if (argc != 2) 
    ExitWithError("Usage: server <local port>");

  srvSock = CreateServer(atoi(argv[1]));

  while (1) {
    FD_ZERO(&set);
	FD_SET(STDIN_FILENO, &set);
	FD_SET(srvSock, &set);

    /********************** SELECT ****************************/
	if (select(FD_SETSIZE, &set, NULL, NULL, NULL) < 0) {
	  WriteError("select() failed");
	  break;
    }

    /********************** -> STDIN ****************************/
    if (FD_ISSET(STDIN_FILENO, &set)) {
	  scanf("%99[^\n]%*c", stdin_string);
	  if (!strncmp(stdin_string, "FIM", 3)) break;
	}

    /********************** -> SOCKET ****************************/
	if (FD_ISSET(srvSock, &set)) {
      if (tid == NTHREADS)
	    ExitWithError("Number of threads is over");

      cliSock = AcceptConnection(srvSock);

	  if ((args = (TArgs *) malloc(sizeof(TArgs))) == NULL) {
	    ExitWithError("malloc() failed");
	  }

	  args->cliSock = cliSock;
	  getIP(cliSock, args->ip);

	  users_logged++;
      printf("\n|Connection Started\n");
      printf("|-IP: %s\n", args->ip);

	  if (pthread_create(&threads[tid++], NULL, HandleRequest, (void *) args))
	    ExitWithError("pthread_create() failed");
 	}   				    
  }

  for (i=0; i<tid; i++)
    pthread_join(threads[i], NULL);

  return 0;
}