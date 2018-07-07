#include "mysocket.h"


class Node{
public: 
  char *IP;
  unsigned int PORT;
  TSocket sock;

  Node(){}
  Node(char *IP, unsigned int &PORT){}
  ~Node(){}


};