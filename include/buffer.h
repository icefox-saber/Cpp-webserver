#include <vector>

class buffer {
  // private:
  /// @brief 储存数据变量
  std::vector<char> buffer_;
  /// @brief buffer_[readpos_:writepos_]是可读的数据
  std::size_t readpos_;
  /// @brief buffer_[readpos_:writepos_]是可读的数据
  std::size_t writepos_;

  /// @brief 此函数保证能写入len个数据
  /// @param len 保证能写入的数据长度
  void make_sure_writable(std::size_t len);
  /// @brief 申请空间使能写入buffer_
  /// @param len 要求能写入buffer_的数据长度
  void make_space(std::size_t space);

  /// @brief 获取可读出缓冲区的数据长度
  /// @return  可读出缓冲区的数据长度
  std::size_t readableBytes() const;
  /// @brief 获取可写入缓冲区的数据长度
  /// @return  可写入缓冲区的数据长度
  std::size_t writableBytes() const;

  /// @brief 获取可移动的空闲缓冲区长度
  /// @return 可移动的空闲缓冲区长度
  std::size_t PrependableBytes() const;

  /// @brief 获取读操作的起始迭代器的位置
  /// @return 读操作的起始迭代器的位置
  std::vector<char>::const_iterator readposIterator() const;

  /// @brief 获取写操作的起始迭代器的位置
  /// @return 读操作的起始迭代器的位置
  std::vector<char>::iterator writeposIterator();

public:
  /// @brief 初始化缓冲区大小为len
  /// @param len 缓冲区的大小
  buffer(std::size_t len = 1024UL);

  /// @brief 向缓冲区中添加数据
  /// @param buffer 数据起始地址
  /// @param len 数据长度
  /// @return 实际添加的数据数
  std::size_t append(const char *buffer, std::size_t len);

  /// @brief 从缓冲区中获取数据，数据读完就没了
  /// @param buffer 数据存入的地方
  /// @param len 希望读取数据的长度，
  /// @return 实际读取的数据长度
  std::size_t get(char *buffer, std::size_t len);

  /// @brief 从文件描述符中读完
  /// @param fd 需要读取的文件描述符
  /// @param error 错误状态的指针
  /// @return 返回读出的字节数
  ssize_t readFromFD(int fd, int *error);

  /// @brief 向一个文件描述符中写入缓冲区所有内容
  /// @param fd 将要写入的文件描述符
  /// @param error 错误状态的指针
  /// @return 写入的字节数
  ssize_t writeToFD(int fd, int *error);

  /// @brief 清除所有缓冲区数据
  void clear();
  ~buffer();
};
