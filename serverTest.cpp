#include "Server/tcpServer.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>

void sent_file(tcpServer &server, int client_socket, std::string video) {
  std::string sourceFile = std::move(video);
  std::ifstream source(sourceFile, std::ios::binary);
  char buffer[2048];
  int ret = 0;
  int bytes_read = 0;
  while (ret != -1 && source.read(buffer, 2048)) {
    bytes_read = source.gcount();
    ret = server.send(client_socket, buffer, bytes_read);
  }
  source.close();
  server.close_client(client_socket);
}

int main(int argc, char *argv[]) {
  std::cout << getpid() << std::endl;
  const std::string sourceFile = "video.mp4";
  tcpServer server;
  int socket_client;
  int port = std::stoi(argv[1]);
  if (server.initialize(port) == -1) {
    std::cerr << "initialize";
    return 1;
  }

  if (server.listen(3) == -1) {
    std::cerr << "listen";
    return 2;
  }

  while (true) {
    socket_client = server.accept();
    if (socket_client == -1) {
      std::cerr << "send";
      return 3;
    }
    std::thread t1(sent_file, std::ref(server), socket_client, sourceFile);
    t1.detach();
  }

  // server.close_client(socket_client);
  return 0;
}
