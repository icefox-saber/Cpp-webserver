#include "../include/tcpListener.h"
#include "../include/log.h"
#define BUFFER_SIZE 1024

tcpServer::tcpServer() {}
int tcpServer::initialize(uint16_t PORT) {
  server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  port = PORT;
  server_addr.sin_port = htons(port);
  int opt = 1;
  setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt,
             sizeof(opt)); // 端口复用（覆盖）
  setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, &opt,
             sizeof(opt)); // 多线程复用同一端口，负载均衡
  setsockopt(server_socket, IPPROTO_TCP, TCP_NODELAY, &opt,
             sizeof(opt)); // 小报文立即发送，低延迟场景
  int fd =
      bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
  return fd; //<0 means fail
};

int tcpServer::listen(int num) { return ::listen(server_socket, num); }

int tcpServer::accept(sockaddr_in & client_addr) {
  socklen_t client_len = sizeof(client_addr);
  int fd =
      ::accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
  char client_ip[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
  std::string client_ip_string(client_ip);
  //logger::instance().log("get connect ip:{} fd:{}", client_ip_string, fd);
  return fd;
};

int tcpServer::getSocket() { return server_socket; }
in_addr tcpServer::getServerAddr_int() { return server_addr.sin_addr; }

std::string tcpServer::getServerAddr_string() {
  return std::string(inet_ntoa(getServerAddr_int()));
}

int tcpServer::close_server() { return ::close(server_socket); };

tcpServer::~tcpServer() { close_server(); }
