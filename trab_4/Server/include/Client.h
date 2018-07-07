#include "Node.h"

class Client : public Node{
public:
  string UserLogin;
  Client(char *IP, unsigned int PORT ): Node( IP,PORT) { }
  Client(): Node() {}

  bool operator < ( const Client *o) const { return this->UserLogin < o->UserLogin; }
};