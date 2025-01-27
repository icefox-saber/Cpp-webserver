#include "../include/tcpClient.h"

#define BUFFER_SIZE 1024

tcpClient::tcpClient() { buffer = new char[BUFFER_SIZE]; };

int tcpClient::initialize() {
  return client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}
int tcpClient::close() { return ::close(client_socket); };

int tcpClient::connect(const std::string &ip, const uint16_t &port) {
  return connect(ip.c_str(), port);
}

int tcpClient::connect(const char *ip, const uint16_t &port) {
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr(ip);
  return ::connect(client_socket, (struct sockaddr *)&server_addr,
                   sizeof(server_addr));
}

int tcpClient::send(const std::string &msg, size_t num) {
  return send(msg.c_str(), num);
}

int tcpClient::send(void *msg, size_t num) {
  return ::send(client_socket, msg, num, 0);
}

int tcpClient::recv(size_t max_len) {
  if (max_len > BUFFER_SIZE) {
    max_len = BUFFER_SIZE;
  }
  return ::recv(client_socket, buffer, max_len, 0);
}

int tcpClient::getSocket() { return client_socket; }
in_addr_t tcpClient::getServerAddr_int() { return server_addr.sin_addr.s_addr; }

std::string tcpClient::getServerAddr_string() {
  in_addr addr;
  addr.s_addr = getServerAddr_int();
  return std::string(inet_ntoa(addr));
}

char *tcpClient::getBuffer() { return buffer; }

tcpClient::~tcpClient() {
  close();
  delete[] buffer;
}
