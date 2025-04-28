#ifndef HTTPCONN_H
#define HTTPCONN_H

#include "buffer.h"
#include "httpRequest.h"
#include "httpResponse.h"
#include "log.h"
#include <deque>
#include <netinet/in.h>
#include <unistd.h>
/// @brief http连接类，负责管理http连接和请求解析和回复生成
class httpconn {
private:
  /// @brief 连接的文件描述符
  int FD_;

  /// @brief 客户端地址
  sockaddr_in client_addr_;
  /// @brief 接受缓冲区
  buffer read_buffer_;
  /// @brief 发送缓冲区
  buffer write_buffer_;
  /// @brief http请求解析器
  httpRequest requestParser_;
  /// @brief http响应
  httpResponse response_;

private:
public:
  void clear();
  httpconn();
  httpconn(int fd, sockaddr_in client_addr);
  void reset(int fd, sockaddr_in client_addr);
  void process();
  ~httpconn();
};

#endif