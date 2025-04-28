#ifndef TCPCONN_H
#define TCPCONN_H

#include <string>
#include <netinet/in.h>

class tcpConn {
private:
  int FD_;
  sockaddr_in client_addr_;
public:
  /// @brief 构造TCP连接对象
  /// @param fd 连接文件描述符
  /// @param client_addr_ 对方地址
  tcpConn(int fd,sockaddr_in client_addr);

  /// @brief 连接新的客户端
  /// @param fd 连接文件描述符
  /// @param client_addr 客户端地址
  void conn(int fd,sockaddr_in client_addr);
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

  /// @brief 读取客户端消息
  /// @param client_socket 客户端
  /// @param buf 存储消息的buffer
  /// @param max_len 最大读取的字节数
  /// @return 读取的字节数，-1表示错误
  int recv(int client_socket, void *buf, size_t max_len);
  /// @brief 关闭客户端连接
  /// @param client_socket 客户端
  /// @return 系统close(FD)的返回值
  int close();
  /// @brief 关闭连接并销毁
  ~tcpConn();
};


#endif