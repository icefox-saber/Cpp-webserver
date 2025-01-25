#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <string>

#define PORT 8080

int main() {
  int client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (client_socket == -1) {
    std::cerr << "Failed to create socket\n";
    return EXIT_FAILURE;
  }

  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  

  // 连接到服务器
  if (connect(3, (struct sockaddr *)&server_addr,
              sizeof(server_addr)) < 0) {
    std::cerr << "Connection failed\n";
    return EXIT_FAILURE;
  }

  // 发送消息
  const char *message = "Hello, server!4";
  std::string str; 
  char msg[strlen(message)+1];
  send(client_socket, message, std::strlen(message), 0);
  std::cout << "Message sent to server\n";
  recv(client_socket, msg, std::strlen(message), 0);
  // 关闭套接字
  close(client_socket);
  return 0;
}