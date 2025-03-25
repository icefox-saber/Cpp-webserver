#ifndef TCPSERVER_H
#define TCPSERVER_H

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
  sockaddr_in server_addr{};
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
  int accept();
  /// @brief 给客户端发送消息
  /// @param client_socket 代表客户端的文件描述符
  /// @param msg 要发送的消息
  /// @param n 发送的字节数
  /// @return 发送的字节数，-1表示错误
  int send(int client_socket, const std::string &msg, size_t n);
  /// @brief 给客户端发送消息
  /// @param client_socket 代表客户端的文件描述符
  /// @param msg 要发送的消息
  /// @param n 发送的字节数
  /// @return 发送的字节数，-1表示错误
  int send(int client_socket, const void *msg, size_t n);
  /// @brief 获取监听套接字
  /// @return 监听套接字
  int getSocket();
  /// @brief 返回服务器地址
  /// @return 服务器地址
  in_addr getServerAddr_int();
  /// @brief 返回服务器地址
  /// @return 服务器地址
  std::string getServerAddr_string();
  /// @brief 读取客户端消息
  /// @param client_socket 客户端
  /// @param buf 存储消息的buffer
  /// @param max_len 最大读取的字节数
  /// @return 读取的字节数，-1表示错误
  int recv(int client_socket, void *buf, size_t max_len);
  /// @brief 关闭服务器
  /// @return 系统close(FD)的返回值
  int close_server();
  /// @brief 关闭客户端连接
  /// @param client_socket 客户端
  /// @return 系统close(FD)的返回值
  int close_client(int client_socket);
  ~tcpServer();
};

#endif
