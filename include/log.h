#include "blockqueue.h"
#include "buffer.h"
#include <condition_variable>
#include <fstream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>

class logger {
private:
  logger(std::string file_ = "./log");
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
  blockqueue<std::string> blockqueue_;
  std::ofstream logger_;
  static std::once_flag once_;

  std::thread writethread_;
};
