#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <unistd.h>

/// @brief tcp协议客户端
class tcpClient {
private:
  /// @brief 客户端的文件描述符
  int client_socket;
  /// @brief 服务器地址
  sockaddr_in server_addr;
  /// @brief 默认recv到buffer指针处
  char *buffer;

public:
  /// @brief 给buffer分配内存
  tcpClient();
  /// @brief 建立socket套接字
  /// @return socket的文件描述符，-1表示失败
  int initialize();
  /// @brief 连接到服务端
  /// @param ip 服务端ip
  /// @param port 服务端端口
  /// @return 0表示连接成功，-1表示失败
  int connect(const std::string &ip, const uint16_t &port);
  /// @brief 连接到服务端
  /// @param ip 服务端ip
  /// @param port 服务端端口
  /// @return 0表示连接成功，-1表示失败
  int connect(const char *ip, const uint16_t &port);
  /// @brief 给服务器发送消息
  /// @param msg 消息
  /// @param n 消息大小
  /// @return 已发送的消息字节数，或者-1
  int send(const std::string &msg, size_t n);
  /// @brief 给服务器发送消息
  /// @param msg 消息
  /// @param n 消息大小
  /// @return 已发送的消息字节数，或者-1
  int send(const void *msg, size_t n);
  /// @brief 获取客户端socket
  /// @return 客户端socket
  int getSocket();
  /// @brief 获取服务端的整数ip
  /// @return 服务端的ip
  in_addr_t getServerAddr_int();
  /// @brief 获取服务端的字符串ip
  /// @return 服务端的ip
  std::string getServerAddr_string();
  /// @brief 从服务端接收消息
  /// @param max_len 最多接受的消息大小，超过BUFFER_SIZE会被设置成BUFFER_SIZE
  /// @return 接受的消息字节数
  int recv(size_t max_len);
  /// @brief 关闭服务器
  /// @return 系统调用close(socket)的返回值
  int close();

  /// @brief 获取接受数据的Buffer
  /// @return buffer的指针
  char *getBuffer();

  /// @brief 释放buffer内存
  ~tcpClient();
};

#endif