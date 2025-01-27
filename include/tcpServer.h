#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <unistd.h>

class tcpServer {
private:
  int server_socket;
  sockaddr_in server_addr{};
  uint16_t port;
  char *buffer;

public:
  tcpServer();
  int initialize(uint16_t PORT);
  int listen(int num);
  int accept();
  int send(int client_socket, const std::string &msg, size_t n);
  int send(int client_socket, const char *msg, size_t n);
  int getSocket();
  in_addr getServerAddr_int();
  std::string getServerAddr_string();
  int recv(int client_socket, size_t max_len);
  int close_server();
  int close_client(int client_socket);
  char *getBuffer();
  ~tcpServer();
};
