#include "../include/log.h"

logger::logger(const std::string &file_)
    : filenameSuffix_(file_), blockqueue_(10),
      writethread_(&logger::write, this) {}

logger &logger::instance() {
  static logger inst_;
  return inst_;
}

logger::~logger() {}

void logger::log(std::string str) {
  auto now_ = std::chrono::system_clock::now();
  std::time_t time_ = std::chrono::system_clock::to_time_t(now_);
  std::tm *localtime_ =
      std::localtime(&time_); // 该指针指向static变量，无需担心内存泄漏
  char timeBuffer[9];         // HH:MM:SS format
  std::strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", localtime_);
  str += "[" + std::string(timeBuffer) + "] ";
  blockqueue_.push_back(std::move(str));
}

void logger::write() {
  std::string msg;
  auto now_ = std::chrono::system_clock::now();
  std::time_t time_ = std::chrono::system_clock::to_time_t(now_);
  std::tm *localtime_ =
      std::localtime(&time_); // 该指针指向static变量，无需担心内存泄漏
  char dateBuffer[11];        // YYYY.MM.DD format
  std::strftime(dateBuffer, sizeof(dateBuffer), "%Y.%m.%d", localtime_);
  std::string date(dateBuffer);
  logger_.open(filenameSuffix_ + date, std::ios::app);
  while (true) {
    if (updateday(localtime_)) {
      std::strftime(dateBuffer, sizeof(dateBuffer), "%Y.%m.%d", localtime_);
      date = dateBuffer;
      logger_.close();
      logger_.open(filenameSuffix_ + date, std::ios::app);
    }
    blockqueue_.pop_front(msg);
    logger_ << msg << std::endl;
  }
}

bool updateday(std::tm *localtime_) {
  auto now_ = std::chrono::system_clock::now();
  std::time_t time_ = std::chrono::system_clock::to_time_t(now_);
  if (std::localtime(&time_)->tm_mday != localtime_->tm_mday) {
    localtime_ = std::localtime(&time_);
    return true;
  }

  return false;
}