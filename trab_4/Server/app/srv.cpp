#include <pthread.h>
#include "structs.h"
#define MAX_NR_USERS 20
#define MAX_SIZE_NAME 40
#define watch(S1,S2) printf("DEBUG %s %s \n",S1,S2);
using namespace std;
const int INF = 1e6;



class Node{
public:
  char *IP;
  unsigned int PORT;
  TSocket sock;

  Node(){}
  Node(char *IP, unsigned int &PORT){
    this->IP = IP;
    this->PORT = PORT;
  }
  ~Node(){}

};

class Client : public Node{
public:
  string UserLogin;
  Client(char *IP, unsigned int PORT ): Node( IP,PORT) { }
  Client(): Node() {}

  bool operator < ( const Client *o) const { return this->UserLogin < o->UserLogin; }
};

void getIP(TSocket sock, char* A){
  unsigned int cliLen;
  struct sockaddr_in cliAddr;
  memset((void *) &cliAddr, 0, sizeof(cliAddr));
  cliLen = sizeof(cliAddr);
  
  if (getpeername(sock, (struct sockaddr *) &cliAddr, &cliLen)) {
    ExitWithError("Error in getting IP");
  }
  strcpy(A,inet_ntoa(cliAddr.sin_addr));
}

// // //Making consts for the programmer cannot modify this pointer here.

class Server : public Node
{


public:

  pthread_t threads[NUMTHREADS];
  static int subscription;
  static pthread_mutex_t mutex;
  static map<char*, Client*> IpToClient;
  static set< Client *> clients;
  static set< string >  logins_used;
  static map< string, Client* > name_refer;
  static map< string, set< pair< string , int >  > >followers;
  static map< int, pair< string, pair< string,int > > > subs; // number subscription to ( follower, follow );
  int tid; // Thread_ids;

  Server(){}
  Server(char *IP, unsigned int PORT) : Node( IP,PORT) { }
  ~Server(){}


  void Create(){
    this->sock = CreateServer(this->PORT);
  }
 static int ResponseOkUser(const TSocket &sock,const int &subs);
 static int addUser( void* client , char *buffer);
 static void ResponseExcepetionUser(const int &op, const TSocket &sock);
 static void* HandleRequest(void* args);
 static int Subscribe(void* client, char *buffer);
 static int Publish(void* client, char *buffer_args);
 static int DeleteProfile(void* client, char*buffer);
 static int CancelSubscription(void* client, char* buffer);
 void Connection();
};



void Server::Connection(){
    void* retval;
    while( true ){
      if(tid == NUMTHREADS)
        ExitWithError("Number of threads is over");

      puts("Waitting Connection...");


      // Check if the client already exist!!!! cuz the client could change his name! or something else.
      // Estou criando um novo Client sempre... ruim <, para cada conexão!
      Client *a = new Client() ;

      int sock_aux = AcceptConnection( this->sock  );
      char IP[IPSIZE];
      getIP( sock_aux ,IP );
      printf("IP::: %s\n",IP);
       // if(IpToClient.find( IP ) != IpToClient.end() ){
       //   a = IpToClient[IP];
       //   a->sock = sock_aux;
       //   a->IP = IP;
       // }
       // else{
        a->IP = IP;
        a->sock = sock_aux;
        IpToClient[IP] = a;
      // }
      printf("\n|Connection Started\n");
      printf("|-IP: %s\n", a->IP);
      printf("sock: %d\n",a->sock);

      if(pthread_create(&threads[tid++], NULL, (THREADFUNCPTR) &Server::HandleRequest ,a) )
        ExitWithError("pthread_create() failed");

      }
    }

void Server:: ResponseExcepetionUser(const int &op, const TSocket &sock){
  if(op == 0){
    printf("ENVIANDO EXCEPTION PARA CONEXAO: %d\n",sock);
    if(WriteN(sock, "0 \n", 5) < 0 )
      ExitWithError("WriteN() failed");
  }
}

void* Server:: HandleRequest(void* args){
    char buffer[ BUFSIZE ],user_option;
    
    Client *A = (Client* )args;
    printf("Sock: %d, IP sendo tratado: %s, nome sendo tratado: %s\n",A->sock,A->IP,A->UserLogin.c_str());
    if (ReadLine(A->sock, buffer, BUFSIZE) < 0) {
      printf("\nRequest message can't be read: %d, IP: %s\n",A->sock,A->IP);
      //ResponseExcepetionUser(0, A->sock);

      return NULL;
    }
    user_option = buffer[0];
    int ok;
    switch(user_option){
      case '1':
        ok = addUser( args , buffer);
        if(ok == 1){
          printf("\n+\n+ %s\n+ ADD\n+\n", A-> IP);
          ResponseOkUser(A->sock,ok);
        }
        else{
          printf("\n+\n+ %s\n+ CANNOT ADD\n+\n", A-> IP);
          ResponseExcepetionUser(ok, A->sock);
        }
        break;
      case '2':
        ok =  Subscribe( args, buffer );
        if(ok != 0){
          printf("\n+\n+ %s\n+ ADD\n+\n", A-> IP);
          ResponseOkUser(A->sock,ok);
        }
        else{
          printf("\n+\n+ %s\n+ CANNOT FIND SUBSCRIBER\n+\n", A-> IP);
          ResponseExcepetionUser(ok, A->sock);
        }
        break;
      case '3':
        ok = CancelSubscription(args,buffer);
        if(ok != 0){
          printf("\n+\n+ %s\n+ Canceled Subscription\n+\n", A-> IP);
          ResponseOkUser(A->sock,ok);
        }
        else{
          printf("\n+\n+ %s\n+ CANNOT FIND SUBSCRIBER\n+\n", A-> IP);
          ResponseExcepetionUser(ok, A->sock);
        }
        // O que quer dizer com codigo da subscricao, pensei que o nome era a chave primaria visto que so pode ter 1.
         break;
      case '4':
        ok = Publish( args, buffer );
        // if(ok){
        //   printf("\n+\n+ %s\n+ PUBLISH\n+\n", A-> IP);
        //   ResponseOkUser(A->sock,ok);
        // }
        // else{
        //   printf("\n+\n+ %s\n+ CANNOT PUBLISH\n+\n", A-> IP);
        //   ResponseExcepetionUser(ok, A->sock);
        // }

        break;
      case '5':
        ok = DeleteProfile(args,buffer);
        if(ok){
          printf("\n+\n+ %s\n+ DELETE\n+\n", A-> IP);
          ResponseOkUser(A->sock,ok);
        }
        else{
          printf("\n+\n+ %s\n+ CANNOT DELETE\n+\n", A-> IP);
          ResponseExcepetionUser(ok, A->sock);

        }
      break;
      default:
      printf("\nInvalid option in user\n");
      if (WriteN(A->sock, "0\n", 2) <= 0)
        ExitWithError("WriteN() in ResponseAllUsers failed");

    }

    printf("\n|Connection Closed\n");
    printf("|-IP: %s\n", A->IP);
    printf("sock: %d\n",A->sock);
    close(A->sock);
    pthread_exit(NULL);
  }



int Server::DeleteProfile(void* client, char *buffer){
    Client *A = ( (Client *) client); // ma'king a cast for a object
    // Modularizar o parser, fazer uma funcao para isso!
    string parser(buffer);
    stringstream ss(parser); //stream that will parser
    vector<string> v;        // vector with the strings;
    while(ss >> parser){ v.pb(parser); }

    string UserLogin = v[1];

    if(logins_used.find(UserLogin) == logins_used.end())
      return 0;
    //watch("DeleteProfile", UserLogin.c_str())
    logins_used.erase( logins_used.find(UserLogin) );
    followers.erase( followers.find(UserLogin) );

    if(IpToClient.find(A->IP) != IpToClient.end() )
      IpToClient.erase( IpToClient.find( A->IP )  );
    return 1;
}

int Server::Publish(void* client, char *buffer_args){
    Client *A = ( (Client *) client); // making a cast for a object
    // Modularizar o parser, fazer uma funcao para isso!
    string parser(buffer_args);
    stringstream ss(parser); //stream that will parser
    vector<string> v;        // vector with the strings;
    while(ss >> parser){ v.pb(parser); }

    string UserLogin = v[1];
    string MSG = "";

    for(int i = 2;i<v.size();i++)
      MSG += v[i] + " ";
    MSG += '\n';
    cout << MSG;

    if( logins_used.find(UserLogin) == logins_used.end() ){
      printf("\n+\n+ %s\n+ CANNOT PUBLISH\n+\n", A-> IP);
      ResponseExcepetionUser(0,A->sock);
      return 0;
    }
    printf("\n+\n+ %s\n+ PUBLISH\n+\n", A-> IP);
    MSG = UserLogin + ": " + MSG;

    char buffer[BUFSIZE];
    strcpy(buffer,MSG.c_str() );
    // Possivel paralelizar essa parte do envio!
    cout << "MSG: " + string(buffer) << " FIM DA MENSAGEM"<< endl;
    
    for(auto user: followers[UserLogin]){
      //Abro uma conexao com cada um, e envio a MSG, posso fazer essa parte em paralelo
      // Client *follow = new Client();
      // follow = name_refer[ user.st ];
      char IP[BUFSIZE];
      strcpy(IP, user.st.c_str());
      int PORT = user.nd;
      cout << "Enviando mensagem para: " + user.st + " ...." << endl;


      cout << "Enviando para IP: " << IP <<   " PORTA " + PORT << endl;
      TSocket sender = ConnectToServer(IP ,PORT);

      if(WriteN(sender, buffer , BUFSIZE ) < 0){
        ExitWithError("WriteN Failed()");
      }

      close(sender);
      cout << "Enviou mensagem para: " + user.st << endl;
    }
    cout << "Enviando mensagem para finalizar conexao com o Publisher: " << A->sock << " " << 1 << endl;
    ResponseOkUser(A->sock,1);
    return 1;

}

int Server::Subscribe(void* client, char* buffer){
    Client *A = ( (Client *) client); // making a cast for a object
    string parser(buffer);
    stringstream ss(parser); //stream that will parser
    vector<string> v;        // vector with the strings;
    while(ss >> parser){ v.pb(parser); }

    if(v.size() != 3)
      return 2;

    string follow = v[1];
    string port   = v[2];

    if(logins_used.find(follow) == logins_used.end())
      return 0;

    int s; // variavel auxiliar

    s = subscription++;
    cout << "subscrição valor: " << s << " mapeamento: " << follow << " " << A->IP << " " << port << endl;
    subs[s] = mp(follow, mp(A->IP,stoi(port) )); // a subscrição guarda IP e PORTA
    followers[follow].insert( mp( A->IP, stoi(port) )) ; // nome que sigo, mapeia, (NOME,PORTA)

    return s;
}
int Server::CancelSubscription(void*client, char*buffer){
    Client *A = ( (Client *) client); // making a cast for a object
    string parser(buffer);
    stringstream ss(parser); //stream that will parser
    vector<string> v;        // vector with the strings;
    while(ss >> parser){ v.pb(parser); }

    int s;
    s = stoi(v[1]);
    if( subs.find(s) == subs.end() ){
      return 0;
    }



    string follow = subs[s].st;
    string UserIP = subs[s].nd.st;
    int PORT      = subs[s].nd.nd;
    set< pair<string,int> >::iterator it;

    it = followers[follow].lower_bound( mp(UserIP, PORT)  );

    followers[follow].erase(  it  );
    subs.erase(  subs.find(s)  );

    return 1;
}
int Server::addUser( void* client , char *buffer){
    Client *A = ( (Client *) client); // making a cast for a object
    string parser(buffer);
    parser = parser.substr(2); // Avoid operation and the first space
    cout << parser << endl;
    int pos_space = parser.find(" ");
    if(pos_space == string::npos){
      return 2;
    }
    string name = parser.substr(0,pos_space);

    if(logins_used.find(name) != logins_used.end() )
      return 0;

    A->UserLogin = name;
    logins_used.insert(name);
    clients.insert(A);
    name_refer[name] = A;

    return 1;
}

int Server::ResponseOkUser(const TSocket &sock, const int &subs){
  char buffer[240];
  string val = "1 " + to_string(subs) + " \n";
  for(int i = 0;i<val.size();i++)
    buffer[i] = val[i];
  int n = val.size();
  buffer[n] = '\0';
  cout << "Response: " << buffer;
  if(WriteN(sock, buffer , sizeof(buffer)) < 0 )
    ExitWithError("WriteN() failed");
  puts("FIM RESPONSE");
}


/*Undefined reference for the class*/
set<Client *> Server::clients; // Undefined reference solved here, conjunto que armazena todos os clients <
set< string > Server::logins_used;
map<string, Client*> Server::name_refer; //mapeia nome->informaçoes, mapa dado o nome, retorna as informações
map< string, set< pair< string , int >  > > Server::followers;
map<char*, Client*> Server::IpToClient;
map< int, pair< string, pair< string,int > > > Server ::subs;
int Server::subscription = 0;
pthread_mutex_t Server::mutex =  PTHREAD_MUTEX_INITIALIZER;
int main(int argc, char ** argv){

  if (argc != 3) {
   ExitWithError("Usage: client <remote server IP> <remote server Port> ");
  }

  Server *srv = new Server(argv[1], atoi(argv[2] ));
  srv->Create();
  srv->Connection();


  delete srv;
  return 0;
}
