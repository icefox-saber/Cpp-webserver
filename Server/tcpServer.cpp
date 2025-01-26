#include <arpa/inet.h>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <mutex>
#include <thread>
#include <unistd.h>
#include <vector>

constexpr int PORT = 8080;
constexpr int BUFFER_SIZE = 1024;

std::mutex cout_mutex; // 保护 std::cout，防止多线程输出混乱
int server_socket;

void handle_client(int client_socket) {
  char buffer[BUFFER_SIZE];

  while (true) {
    memset(buffer, 0, BUFFER_SIZE);
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (bytes_received <= 0) {
      std::lock_guard<std::mutex> lock(cout_mutex);
      std::cout << "Client disconnected." << std::endl;
      break;
    }

    {
      std::lock_guard<std::mutex> lock(cout_mutex);
      std::cout << "Received: " << buffer << std::endl;
    }

    send(client_socket, buffer, bytes_received, 0);
  }

  close(client_socket);
}
std::vector<std::thread> client_threads;

int main() {

  server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (server_socket == -1) {
    std::cerr << "Failed to create socket\n";
    return EXIT_FAILURE;
  }

  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(PORT);
  int fd =
      bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (fd < 0) {
    std::cerr << "Bind failed\n";
    close(server_socket);
    return EXIT_FAILURE;
  }

  if (listen(server_socket, 5) < 0) {
    std::cerr << "Listen failed\n";
    close(server_socket);
    return EXIT_FAILURE;
  }

  std::cout << "Server listening on port " << PORT << "..." << std::endl;
  std::signal(SIGINT, [](int) {
    std::signal(SIGINT,SIG_IGN);
    std::cout<<"dds"<<std::endl;
    for (auto &thread : client_threads) {
      if (thread.joinable()) {
        thread.join();
      }
    }
    close(server_socket);
    //return;
    exit(0);
  });
  while (true) {
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    int client_socket =
        accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
    if (client_socket < 0) {
      std::cerr << "Accept failed\n";
      continue;
    }

    {
      std::lock_guard<std::mutex> lock(cout_mutex);
      std::cout << "New connection from " << inet_ntoa(client_addr.sin_addr)
                << std::endl;
    }
    client_addr.sin_addr.s_addr = 0;
    client_threads.emplace_back(handle_client, client_socket);
  }

  for (auto &thread : client_threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }

  close(server_socket);
  return EXIT_SUCCESS;
}
