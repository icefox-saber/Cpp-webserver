#include "buffer.h"
#include <condition_variable>
#include <fstream>
#include <memory>
#include <mutex>
#include <string>
#include <utility>

class logger {
private:
  logger(std::string file_ = "./log");
  logger(const logger &) = delete;
  logger(logger &&) = delete;
  logger &operator=(const logger &) = delete;
  logger &operator=(logger &&) = delete;

public:
  ~logger();
  static logger *instance();
  void log(std::string str);

private:
  static std::ofstream logger_;
  static std::mutex mtx_;
  static std::once_flag once;
  static std::unique_ptr<logger> inst;
};
