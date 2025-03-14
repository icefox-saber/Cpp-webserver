#include "../include/log.h"

std::ofstream logger::logger_;
std::mutex logger::mtx_;
std::once_flag logger::once;
std::unique_ptr<logger> logger::inst;

logger::logger(std::string file_) { logger_.open(file_, std::ios::app); }

logger *logger::instance() {
  std::call_once(once, []() { inst.reset(new logger()); });

  return inst.get();
}

logger::~logger() {
  std::unique_lock lck(mtx_);
  if (logger_.is_open()) {
    logger_.close();
  }
}

void logger::log(std::string str) {
  std::unique_lock lck(mtx_);
  logger_ << str << std::endl;
}