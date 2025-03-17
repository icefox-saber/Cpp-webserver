#include "../include/server.h"

server::server(int trigmod, uint16_t PORT, size_t threadsnum, int timeouts,
               int maxevent, int listennum)
    : epoller_(maxevent, timeouts), threadPoller_(threadsnum), tcpServer_(),
      triggerMod_(trigmod) {
  // 初始化监听套接字
  logger::instance()->log("tcpServer initialize" +
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
    // logger::instance()->log("epoll wait " + std::to_string(infds) + "event");
    if (infds < 0) {
      logger::instance()->log("epoll_wait fial");
    } // 错误

    else if (infds == 0) {
      // logger::instance()->log("epoll wait time out");
    } // 超时
    else {
      for (int i = 0; i < infds; i++) {
        logger::instance()->log("epoll" + std::to_string(i) + "event");
        int sock = epoller_.getFd(i);
        auto event = epoller_.getEvent(i);
        if (sock == listensock_) {
          logger::instance()->log("listen event");
          dealListen();
        } else if (event & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
          logger::instance()->log("close event");
          dealClose(sock);
        } else if (event & EPOLLIN) {
          logger::instance()->log("read event");
          dealRead(sock);
        } else if (event & EPOLLOUT) {
          logger::instance()->log("write event");
          dealWrite(sock);
        } else {
          logger::instance()->log("unexpected error in server::run");
        }
      }
    }
  }
}

void server::dealListen() {
  while (listenEvent_ & EPOLLET) {
    int fd = tcpServer_.accept();
    if (fd < 0) {
      logger::instance()->log("accept all listen fd");
      break;
    }
    SetFdNonblock(fd);
    epoller_.add_fd(fd, connEvent_);
    logger::instance()->log("accept a fd " + std::to_string(fd));
  }
}

void server::dealClose(int fd) {
  epoller_.del_fd(fd, connEvent_);
  tcpServer_.close_client(fd);
}
void server::dealRead(int fd) {
  threadPoller_.emplace(&server::handleclient, this, fd);
}
void server::dealWrite(int fd) {
  logger::instance()->log("no write handler but write event happen");
}

void server::handleclient(int fd) {
  while (connEvent_ & EPOLLET) {
    char buffer[1024];
    int len = tcpServer_.recv(fd, buffer, 1024);
    logger::instance()->log("recv " + std::to_string(len) + " bytes from fd " +
                            std::to_string(fd));
    if (len > 0) {
      tcpServer_.send(fd, buffer, len);
    } else {
      logger::instance()->log("all msg from fd recv");
      break;
    }
  }
  epoller_.mod_fd(fd, connEvent_);
}
void server::SetFdNonblock(int fd) {
  fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
}

server::~server() {}
