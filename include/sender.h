#ifndef SENDER_H
#define SENDER_H

#include "tcpServer.h"
#include "threadPool.h"
#include <fstream>

/// @brief tcp协议的文件发送类
class sender : private tcpServer {
private:
  /// @brief 处理客户端线程池，如果使用必须保证任务队列里的任务是线程安全的
  threadPool threads;
  /// @brief 处理客户端的函数，用于添加到线程池的任务队列中
  /// @param client_socket 客户端套接字的文件描述符
  void handle_client(int client_socket);

public:
  /// @brief 构造函数，完成线程池初始化和监听
  /// @param thread_num 线程池大小，默认为5
  sender(size_t thread_num = 5);
  /// @brief
  /// 服务器初始化，在端口PORT完成监听，运行已连接未处理的监听队列大小为listen_num+1
  /// @param PORT 监听端口号
  /// @param listen_num 监听队列大小
  /// @return -1表示失败，0表示成功
  int initialize(uint16_t PORT, size_t listen_num = 0);
  /// @brief 运行服务器，即循环处理已连接的客户端
  void run();
  ~sender();
};

#endif