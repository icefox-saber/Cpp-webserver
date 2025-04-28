#ifndef SERVER_H
#define SERVER_H

#include "epoll.h"
#include "log.h"
#include "tcpServer.h"
#include "threadPool.h"
#include <fcntl.h>
#include <iostream>
// #include <format>

class server {
private:
  /// @brief 管理Epoll的对象
  Epoll epoller_;
  /// @brief 线程池
  threadPool threadPoller_;
  /// @brief 封装tcp协议的对象
  tcpServer tcpServer_;
  /// @brief 触发模式
  int triggerMod_;
  /// @brief 监听套接字
  int listensock_;
  /// @brief 监听套接字的相应事件类型
  uint32_t listenEvent_;
  /// @brief 连接套接字的相应事件类型
  uint32_t connEvent_;

public:
  /// @brief 服务器初始化
  /// @param trigmod
  /// 触发模式，0表示水平触发，1表示监听套接字边缘触发，2表示连接套接字边缘触发，其余值全部都是边缘触发
  /// @param PORT 监听端口号
  /// @param threadsnum 线程池线程数
  server(int trigmod = 0, uint16_t PORT = 8080, size_t threadsnum = 2,
         int timeouts = -1, int maxevent = 8, int listennum = 1);

  /// @brief 服务器运行
  void run();
  ~server();

private:
  /// @brief 处理监听事件
  void dealListen();
  /// @brief 处理客户端断开事件
  /// @param fd 客户端连接的FD
  void dealClose(int fd);
  /// @brief 处理读事件
  /// @param fd 客户端连接的FD
  void dealRead(int fd);
  /// @brief 处理写事件
  /// @param fd 客户端连接的FD
  void dealWrite(int fd);
  /// @brief 处理客户端的函数
  /// @param fd 客户端的fd
  void handleclient(int fd);
  /// @brief 设置FD为非阻塞
  /// @param fd 要设置的FD
  void SetFdNonblock(int fd);
};

#endif