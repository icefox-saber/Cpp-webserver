#ifndef SENDER_H
#define SENDER_H

#include "tcpServer.h"
#include "threadPool.h"
#include <fstream>

class sender : private tcpServer {
private:
  threadPool threads;
  void handle_client(int client_socket);

public:
  sender(size_t num = 1);
  int initialize(uint16_t PORT);
  void run();
  ~sender();
};

#endif