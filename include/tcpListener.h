#ifndef TCPLISTENER_H
#define TCPLISTENER_H

#include <arpa/inet.h>
#include <iostream>
#include <netinet/tcp.h>
#include <string>
#include <unistd.h>

/// @brief tcp协议服务端
class tcpServer {
private:
  /// @brief 监听套接字文件描述符
  int server_socket;
  /// @brief 服务端地址
  sockaddr_in server_addr;
  /// @brief 服务端端口
  uint16_t port;

public:
  /// @brief 构造函数
  tcpServer();
  /// @brief 初始化服务器
  /// @param PORT 服务器端口号
  /// @return <0表示出错
  int initialize(uint16_t PORT);
  /// @brief 开始监听
  /// @param num 允许未接受的连接最大个数
  /// @return -1表示错误
  int listen(int num = 1);
  /// @brief 接受一个TCP连接
  /// @return 连接套接字文件描述符
  int accept(sockaddr_in & addr);
  
  int getSocket();
  /// @brief 返回服务器地址
  /// @return 服务器地址
  in_addr getServerAddr_int();
  /// @brief 返回服务器地址
  /// @return 服务器地址
  std::string getServerAddr_string();
  
  /// @brief 关闭服务器
  /// @return 系统close(FD)的返回值
  int close_server();
  ~tcpServer();
};

#endif
