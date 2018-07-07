#include "Client.h"
#define NUMTHREADS 100

class Server : public Node
{


public:
  pthread_t threads[NUMTHREADS];
  static set< Client *> clients; 
  static set< string >  logins_used;
  static map< string, Client* > name_refer;
  int tid; // Thread_ids;
  
  Server(){}
  Server(char *IP, unsigned int PORT) : Node( IP,PORT) { }
  ~Server(){}


  void Create(){
    this->sock = CreateServer(this->PORT);
  }
 static int ResponseOkUser(const TSocket &sock);
 static int addUser( void* client , char *buffer);
 static void ResponseExcepetionUser(const int &op, const TSocket &sock);
 static void* HandleRequest(void* args);
 void Connection();  
};
