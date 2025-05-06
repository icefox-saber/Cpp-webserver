#ifndef LOG_H
#define LOG_H

#include "blockqueue.h"
#include <chrono>
#include <condition_variable>
#include <format>
#include <fstream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>

/// @brief 单例的日志，用C++文件流实现
class logger {
private:
  /// @brief 打开一个文件日志
  /// @param file_ 日志文件路径前缀
  logger(const std::string &filename = "./.Log/");
  logger(const logger &) = delete;
  logger(logger &&) = delete;
  logger &operator=(const logger &) = delete;
  logger &operator=(logger &&) = delete;
  /// @brief 从消息队列中一直取消息写到日志中
  void write();

public:
  ~logger();
  /// @brief 获取单例日志的对象引用，因为是static变量，不用担心引用失效
  /// @return 日志的引用
  static logger &instance();
  /// @brief 写入日志
  /// @param str 写入日志的内容
  void log(std::string_view str);
  template <typename... _Args>
  void log(std::string_view __fmt, _Args &&...__args);

private:
  /// @brief 日志文件路径的前缀
  const std::string filenameSuffix_;
  /// @brief 线程安全的消息队列
  blockqueue<std::string> blockqueue_;
  /// @brief 刷磁盘的日志线程
  std::thread writethread_;
  /// @brief 日志文件流对象
  std::ofstream logger_;
};

/// @brief 
/// @tparam ..._Args 
/// @param __fmt 
/// @param ...__args 
template <typename... _Args>
void logger::log(std::string_view __fmt,_Args &&...__args) {
  return log(std::vformat(__fmt, std::make_format_args(__args...)));
}

/// @brief 更新日期的函数
/// @param mday 旧的天
/// @return ture表示日期更新了，false表示没变
bool updateday(int mday);

/// @brief 
/// @tparam ..._Args 
/// @param __fmt 
/// @param ...__args 
template <typename... _Args>
void log(std::string_view __fmt, _Args &&...__args) {
  return logger::instance().log(__fmt, std::forward<_Args>(__args)...);
}

#endif