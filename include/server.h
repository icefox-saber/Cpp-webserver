#ifndef SERVER_H
#define SERVER_H

#include "buffer.h"
#include "epoll.h"
#include "log.h"
#include "tcpServer.h"
#include "threadPool.h"
#include <fcntl.h>
#include <iostream>
// #include <format>

class server {
private:
  Epoll epoller_;
  threadPool threadPoller_;
  tcpServer tcpServer_;
  int triggerMod_;
  int listensock_;
  uint32_t listenEvent_;
  uint32_t connEvent_;

public:
  /// @brief 服务器初始化
  /// @param trigmod
  /// 触发模式，0表示水平触发，1表示监听套接字边缘触发，2表示连接套接字边缘触发，其余值全部都是边缘触发
  /// @param PORT 监听端口号
  /// @param threadsnum 线程池线程数
  server(int trigmod = 0, uint16_t PORT = 8080, size_t threadsnum = 2,
         int timeouts = -1, int maxevent = 8, int listennum = 1);

  void dealListen();
  void dealClose(int fd);
  void dealRead(int fd);
  void dealWrite(int fd);
  void handleclient(int fd);
  void SetFdNonblock(int fd);
  void run();
  ~server();
};

#endif