#include "../include/tcpServer.h"

#define BUFFER_SIZE 1024

tcpServer::tcpServer() { buffer = new char[BUFFER_SIZE]; }
int tcpServer::initialize(uint16_t PORT) {
  server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  port = PORT;
  server_addr.sin_port = htons(port);
  int opt =1 ;
  setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
  int fd =
      bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
  return fd; //<0 means fail
};

int tcpServer::listen(int num) { return ::listen(server_socket, num); }

int tcpServer::accept() {
  sockaddr_in client_addr{};
  socklen_t client_len = sizeof(client_addr);
  return ::accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
};

int tcpServer::send(int client_socket, const std::string &msg, size_t num) {
  return send(client_socket, msg.c_str(), num);
}

int tcpServer::send(int client_socket, const void *msg, size_t num) {
  return ::send(client_socket, msg, num, 0);
}

int tcpServer::recv(int client_socket, size_t max_len) {
  if (max_len > BUFFER_SIZE) {
    max_len = BUFFER_SIZE;
  }
  return ::recv(client_socket, buffer, max_len, 0);
}

int tcpServer::recv(int client_socket, size_t max_len , void *buf)
{
  return ::recv(client_socket, buf, max_len, 0);
}

int tcpServer::getSocket() { return server_socket; }
in_addr tcpServer::getServerAddr_int() { return server_addr.sin_addr; }

std::string tcpServer::getServerAddr_string() {
  return std::string(inet_ntoa(getServerAddr_int()));
}

char *tcpServer::getBuffer() { return buffer; }
int tcpServer::close_client(int client_socket) {
  return ::close(client_socket);
};

int tcpServer::close_server() { return ::close(server_socket); };

tcpServer::~tcpServer() {
  close_server();
  delete[] buffer;
}
