#include "../include/httpConn.h"
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BACKLOG 5

int main() {
  // 创建监听套接字
  int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_fd == -1) {
    perror("Socket creation failed");
    return -1;
  }

  // 设置地址复用
  int opt = 1;
  setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  // 绑定地址和端口
  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  if (bind(listen_fd, (sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    perror("Bind failed");
    close(listen_fd);
    return -1;
  }

  // 开始监听
  if (listen(listen_fd, BACKLOG) == -1) {
    perror("Listen failed");
    close(listen_fd);
    return -1;
  }

  std::cout << "Server is listening on port " << PORT << "...\n";

  while (true) {
    // 接受客户端连接
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(listen_fd, (sockaddr *)&client_addr, &client_len);
    if (client_fd == -1) {
      perror("Accept failed");
      continue;
    }

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    std::cout << "Accepted connection from " << client_ip << ":"
              << ntohs(client_addr.sin_port) << "\n";

    // 创建 httpconn 对象处理连接
    httpconn connection(client_fd, client_addr);
    connection.process();

    // 关闭客户端连接
    close(client_fd);
    std::cout << "Connection closed\n";
  }

  // 关闭监听套接字
  close(listen_fd);
  return 0;
}