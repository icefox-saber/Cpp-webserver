#include "../include/tcpConn.h"
#include <sys/socket.h>
#include <unistd.h>

tcpConn::tcpConn(int fd, sockaddr_in client_addr)
    : FD_(fd), client_addr_(client_addr_) {}

void tcpConn::conn(int fd, sockaddr_in client_addr) {
  close();
  FD_ = fd;
  client_addr_ = client_addr;
}

int tcpConn::send(int client_socket, const std::string &msg, size_t num) {
  return send(client_socket, msg.c_str(), num);
}

int tcpConn::send(int client_socket, const void *msg, size_t num) {
  return ::send(client_socket, msg, num, 0);
}

int tcpConn::recv(int client_socket, void *buf, size_t max_len) {
  return ::recv(client_socket, buf, max_len, 0);
}

int tcpConn::close() { return ::close(FD_); };

tcpConn::~tcpConn() { close(); }