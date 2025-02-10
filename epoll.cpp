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

  int port = atoi(argv[1]);

  int listenfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
  sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(port);
  int opt = 1;
  setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
  setsockopt(listenfd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
  int fd = bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (listen(listenfd, 64) != 0) {
    cerr << "listen";
    return 1;
  }

  int epollfd =
      epoll_create1(EPOLL_CLOEXEC); //执行exec系列函数时，epollfd会自动关闭
  epoll_event ev;
  ev.events = EPOLLIN;
  ev.data.fd = listenfd;
  epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev);

  epoll_event evs[10];

  while (true) {
    int infds = epoll_wait(epollfd, evs, 10, -1);

    if (infds < 0) {
      cerr << "epoll_wait fial\n";
      return 1;
    } //错误

    else if (infds == 0) {
      cout << "epoll wait time out\n";
    } //超时
    else {
      for (size_t i = 0; i < infds; i++) {
        if (evs[i].events == EPOLLIN) {     //有读事件
          if (evs[i].data.fd == listenfd) { //监听套接字有读事件
            sockaddr_in client_addr{};
            socklen_t client_len = sizeof(client_addr);
            int clientfd = accept4(listenfd, (struct sockaddr *)&client_addr,
                                   &client_len, SOCK_NONBLOCK);
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip,
                      INET_ADDRSTRLEN);
            cout << "client connected: " << client_ip << endl;
            epoll_event ev;
            ev.events = EPOLLIN | EPOLLET; //边缘触发
            ev.data.fd = clientfd;
            epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &ev);

          }                                 //监听套接字有读事件
          else {                            //客户端有读事件
            if (evs[i].events & EPOLLHUP) { //客户端断开
              cout << "client" << evs[i].events << "disconnected" << endl;
              close(evs[i].data.fd);
              break;
            }                                                //客户端断开
            else if (evs[i].events & (EPOLLIN | EPOLLPRI)) { //客户端有消息
              char buffer[1024];

              while (true) {
                std::memset(buffer, 0, sizeof(buffer));
                int num = recv(evs[i].data.fd, buffer, sizeof(buffer),
                               0); //边缘触发等于-1
                if (num > 0) {
                  send(evs[i].data.fd, buffer, sizeof(buffer), num);
                  std::cout << "buffer: " << buffer << endl;
                  num = recv(evs[i].data.fd, buffer, sizeof(buffer), 0);
                  continue;
                } else if (num == -1 && errno == EINTR) { //环境中断
                  continue;
                } else if (num == -1 && (errno == EAGAIN ||
                                         errno == EWOULDBLOCK)) { //数据全部读出
                  cout << "read data from" << evs[i].data.fd << "finish"
                       << endl;
                  break;
                } else if (num == 0) {
                  cout << "client" << evs[i].data.fd << "disconnected" << endl;
                  close(evs[i].data.fd);
                  break;
                } else {
                  cerr << "unexpected wrong in client" << evs[i].data.fd;
                  break;
                }
              }
            }

          }                                    //客户端有读事件
        } else if (evs[i].events & EPOLLOUT) { //有写事件
          // nothing to do
        } else // unexpected error
        {
          cerr << "unexpected error in fd " << evs[i].data.fd << endl;
        }
      }
    }
  }
}