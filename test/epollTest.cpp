#include "../include/epoll.h"
#include "../include/tcpServer.h"
#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/tcp.h> //TCP_NODELAY
#include <string>
#include <sys/epoll.h>
#include <unistd.h>

using std::cerr, std::cout, std::endl, std::cin;

int main(int argc, char *argv[]) {
  if (!argv[1]) {
    cerr << "using: ./epoll <port> eg. ./epoll 8080\n";
    return 1;
  }

  tcpServer server{};
  int port = atoi(argv[1]);
  server.initialize(port);
  server.listen(5);
  Epoll epoller{};
  epoller.add_fd(server.getSocket(), EPOLLIN);

  while (true) {
    int infds = epoller.wait();
    if (infds < 0) {
      cerr << "epoll_wait fial\n";
      return 1;
    } // 错误

    else if (infds == 0) {
      cout << "epoll wait time out\n";
    } // 超时
    else {
      for (int i = 0; i < infds; i++) {
        if (epoller.getEvent(i) == EPOLLIN) {           // 有读事件
          if (epoller.getFd(i) == server.getSocket()) { // 监听套接字有读事件
            sockaddr_in client_addr{};
            socklen_t client_len = sizeof(client_addr);
            int clientfd =
                accept4(server.getSocket(), (struct sockaddr *)&client_addr,
                        &client_len, SOCK_NONBLOCK);
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip,
                      INET_ADDRSTRLEN);
            cout << "client connected: " << client_ip << endl;
            epoller.add_fd(clientfd, EPOLLIN | EPOLLET);

          }                                       // 监听套接字有读事件
          else {                                  // 客户端有读事件
            if (epoller.getEvent(i) & EPOLLHUP) { // 客户端断开
              cout << "client" << epoller.getFd(i) << "disconnected" << endl;
              close(epoller.getFd(i));
              break;
            } // 客户端断开
            else if (epoller.getEvent(i) &
                     (EPOLLIN | EPOLLPRI)) { // 客户端有消息
              char buffer[1024];

              while (true) {
                std::memset(buffer, 0, sizeof(buffer));
                int num = server.recv(epoller.getFd(i), buffer, sizeof(buffer));
                if (num > 0) {
                  server.send(epoller.getFd(i), buffer, num);
                  std::cout << "buffer: " << buffer << endl;
                  continue;
                } else if (num == -1 && errno == EINTR) { // 环境中断
                  continue;
                } else if (num == -1 &&
                           (errno == EAGAIN ||
                            errno == EWOULDBLOCK)) { // 数据全部读出
                  cout << "read data from" << epoller.getFd(i) << "finish"
                       << endl;
                  break;
                } else if (num == 0) {
                  cout << "client" << epoller.getFd(i) << "disconnected"
                       << endl;
                  close(epoller.getFd(i));
                  break;
                } else {
                  cerr << "unexpected wrong in client" << epoller.getFd(i);
                  break;
                }
              }
            }

          }                                          // 客户端有读事件
        } else if (epoller.getEvent(i) & EPOLLOUT) { // 有写事件
          // nothing to do
        } else // unexpected error
        {
          cerr << "unexpected error in fd " << epoller.getFd(i) << endl;
        }
      }
    }
  }
}