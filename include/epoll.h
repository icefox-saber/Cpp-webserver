#include <cassert>
#include <memory>
#include <sys/epoll.h>
#include <vector>

/// @brief 封装的EPOLL类
class Epoll {
private:
  /// @brief epoll句柄
  int epoll_;
  /// @brief 事件池
  std::vector<epoll_event> events;
  /// @brief 超时的时长(ms)
  int timeout_;

public:
  /// @brief 初始化EPoll对象
  /// @param maxevents 允许一次监听的最大事件数
  /// @param timeout 超时的时长(ms),-1表示无穷大
  Epoll(size_t maxevents = 16, int timeout = -1);
  /// @brief 不允许拷贝
  Epoll(const Epoll &) = delete;
  /// @brief 添加文件描述符到Epoll句柄
  /// @param fd 文件描述符
  /// @param events 要检测的事件
  /// @return true表示添加成功，false表示添加失败
  bool add_fd(int fd, uint32_t events);
  /// @brief 调整文件描述符监听的事件
  /// @param fd 要调整的文件描述符
  /// @param events 要调整成的监听事件
  /// @return true表示成功，false表示失败
  bool mod_fd(int fd, uint32_t events);
  /// @brief 删除文件描述符监听的事件
  /// @param fd 删除的文件描述符
  /// @param events 为了对齐的参数，无用
  /// @return true表示成功，false表示失败
  bool del_fd(int fd, uint32_t events);
  /// @brief 监听epoll中的文件描述符
  /// @param timeout 超时的时长
  /// @return 监听到的事件数
  int wait(int timeout);
  /// @brief 监听epoll中的文件描述符
  /// @return 监听到的事件数
  int wait();
  /// @brief 获取第i个事件的文件描述符
  /// @param index 事件的下标
  /// @return 文件描述符
  int getFd(int index);
  /// @brief 获取第i个事件
  /// @param index 事件的下标
  /// @return 事件
  uint32_t getEvent(int index);
  ~Epoll();
};
