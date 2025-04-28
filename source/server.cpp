#include "../include/server.h"

server::server(int trigmod, uint16_t PORT, size_t threadsnum, int timeouts,
               int maxevent, int listennum)
    : epoller_(maxevent, timeouts), threadPoller_(threadsnum), tcpServer_(),
      triggerMod_(trigmod) {
  // 初始化监听套接字
  logger::instance().log("tcpServer initialize" +
                         std::to_string(tcpServer_.initialize(PORT)));
  tcpServer_.listen(listennum);
  listensock_ = tcpServer_.getSocket();
  // Set listensock_ to non-blocking mode
  SetFdNonblock(listensock_);

  // 设置触发模式
  listenEvent_ = EPOLLIN;
  connEvent_ = EPOLLONESHOT | EPOLLRDHUP |
               EPOLLIN; // 确保事件只被处理一次，避免多线程环境下的竞争
  switch (triggerMod_) {
  case 0:
    break;
  case 1:
    listenEvent_ |= EPOLLET;
    break;
  case 2:
    connEvent_ |= EPOLLET;
    break;
  default:
    listenEvent_ |= EPOLLET;
    connEvent_ |= EPOLLET;
    break;
  }

  epoller_.add_fd(listensock_, listenEvent_);
}

void server::run() {
  while (true) {
    int infds = epoller_.wait();
    // logger::instance().log("epoll wait " + std::to_string(infds) + "event");
    if (infds < 0) {
      logger::instance().log("epoll_wait fial");
    } // 错误

    else if (infds == 0) {
      // logger::instance().log("epoll wait time out");
    } // 超时
    else {
      for (int i = 0; i < infds; i++) {
        logger::instance().log("epoll" + std::to_string(i) + "event");
        int sock = epoller_.getFd(i);
        auto event = epoller_.getEvent(i);
        if (sock == listensock_) {
          logger::instance().log("listen event");
          dealListen();
        } else if (event & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
          logger::instance().log("close event");
          dealClose(sock);
        } else if (event & EPOLLIN) {
          logger::instance().log("read event");
          dealRead(sock);
        } else if (event & EPOLLOUT) {
          logger::instance().log("write event");
          dealWrite(sock);
        } else {
          logger::instance().log("unexpected error in server::run");
        }
      }
    }
  }
}

void server::dealListen() {
  while (listenEvent_ & EPOLLET) {
    sockaddr_in client_addr{};
    int fd = tcpServer_.accept(client_addr);
    if (fd < 0) {
      log("accept all listen fd");
      break;
    }
    SetFdNonblock(fd);
    epoller_.add_fd(fd, connEvent_);
    httpConnPool_[fd].reset(fd, client_addr);
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    std::string_view str(client_ip, INET_ADDRSTRLEN);
    //log("accept a fd:{} from {}", fd, str);
  }
}

void server::dealClose(int fd) {
  epoller_.del_fd(fd, connEvent_);
  httpConnPool_[fd].clear();
}
void server::dealRead(int fd) {
  threadPoller_.emplace(&server::handleclient, this, fd);
}
void server::dealWrite(int fd) {
  log("no write handler but write event happen");
}

void server::handleclient(int fd) {
  httpConnPool_[fd].process();
  // 处理完后重新注册事件
  epoller_.mod_fd(fd, connEvent_);
}
void server::SetFdNonblock(int fd) {
  fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
}

server::~server() {}
