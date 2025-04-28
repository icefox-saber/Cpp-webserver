#ifndef HTTPCONN_H
#define HTTPCONN_H

#include "buffer.h"
#include "httpRequest.h"
#include "log.h"
#include "httpResponse.h"
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
  void clear();

public:
  httpconn(int fd, sockaddr_in client_addr);
  void reset(int fd, sockaddr_in client_addr);
  void process();
  ~httpconn();
};

void httpconn::clear() {
  close(FD_);
  read_buffer_.clear();
  write_buffer_.clear();
  requestParser_.Init();
  response_.clear();
  FD_ = -1;
}

httpconn::httpconn(int fd, sockaddr_in client_addr)
    : FD_(fd), client_addr_(client_addr) {}

void httpconn::reset(int fd, sockaddr_in client_addr) {
  if (FD_ != -1) {
    clear();
  }
  FD_ = fd;
  client_addr_ = client_addr;
}

void httpconn::process() {
  read_buffer_.readFromFD(FD_, nullptr);
  std::string_view line(read_buffer_.begin().base(), read_buffer_.end().base());
  log("read request\n:{}",line);
  // 如果包体未完成接受
  std::string srcdir = "./resourse";
  requestParser_.Init();
  if (!requestParser_.parse(read_buffer_)) {
    if (requestParser_.isbad()) {
      response_.init(requestParser_.path(), false, 400,srcdir);
      response_.makeResponse(write_buffer_);
      write_buffer_.writeToFD(FD_, nullptr);
      clear();
      return;
    }
    return;
  }

  // 如果是post请求告知不支持
  if (requestParser_.method() == "POST") {
    response_.init(requestParser_.path(), false, 405,srcdir);
  } else {
    response_.init(requestParser_.path(), requestParser_.IsKeepAlive(), 200,srcdir);
  }

  response_.makeResponse(write_buffer_);
  std::string_view line1(write_buffer_.begin().base(), write_buffer_.end().base());
  //log("respose\n:{}",line1);
  // 写入响应
  write_buffer_.writeToFD(FD_, nullptr);
  if (!requestParser_.IsKeepAlive()) {
    clear();
  }

  return;
}

httpconn::~httpconn() {}

#endif