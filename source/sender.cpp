#include "../include/sender.h"



sender::sender(size_t thread_num) : threads(thread_num){};

int sender::initialize(uint16_t PORT ,size_t listen_num ) {
  if (tcpServer::initialize(PORT) == -1) {
    return -1;
  }
  if (tcpServer::listen(listen_num) == -1) {
    return -1;
  }

  return 0;
}

void sender::run() {
  while (true) {
    int client_socket = tcpServer::accept();
    threads.emplace(&sender::handle_client, this, client_socket);
  }
};

void sender::handle_client(int client_socket) {
  struct file_info {
    char filename[256];
    size_t size;
  } file;
  tcpServer::recv(client_socket, sizeof(file), (char *)&file);
  std::string sourceFile = file.filename;
  // size_t file_size = file.size;
  std::ifstream source(sourceFile, std::ios::binary);
  char buffer[2048];
  int ret = 0;
  int bytes_read = 0;
  while (ret != -1 && source.read(buffer, 2048)) {
    bytes_read = source.gcount();
    ret = tcpServer::send(client_socket, buffer, bytes_read);
  }
  source.close();
  tcpServer::close_client(client_socket);
}

sender::~sender(){};
