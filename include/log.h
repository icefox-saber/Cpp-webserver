#include "blockqueue.h"
#include "buffer.h"
#include <condition_variable>
#include <fstream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <chrono>

/// @brief 单例的日志，用C++文件流实现
class logger {
private:
  /// @brief 
  /// @param file_ 
  logger(const std::string &filename = "./.Log/");
  logger(const logger &) = delete;
  logger(logger &&) = delete;
  logger &operator=(const logger &) = delete;
  logger &operator=(logger &&) = delete;
  void write();

public:
  ~logger();
  static logger &instance();
  void log(std::string str);

private:
  const std::string filenameSuffix_;
  blockqueue<std::string> blockqueue_;
  std::thread writethread_;
  std::ofstream logger_;

};

bool updateday(int mday);