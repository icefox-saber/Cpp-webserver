#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <unistd.h>

class tcpClient {
private:
  int client_socket;
  sockaddr_in server_addr;
  char *buffer;

public:
  tcpClient(/* args */);
  int initialize();
  int connect(const std::string &ip, const uint16_t &port);
  int connect(const char *ip, const uint16_t &port);
  int send(const std::string &msg, size_t n);
  int send(const char *msg, size_t n);
  int getSocket();
  in_addr getServerAddr_int();
  std::string getServerAddr_string();
  int recv(size_t max_len);
  int close();
  char *getBuffer();
  ~tcpClient();
};
