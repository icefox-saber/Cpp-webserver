#include "../include/log.h"
#include <iostream>

logger::logger(const std::string &file_)
    : filenameSuffix_(file_), blockqueue_(10),
      writethread_(&logger::write, this) {}

logger &logger::instance() {
  static logger inst_;
  return inst_;
}

logger::~logger() {}

void logger::log(std::string_view str_) {
  std::string str(str_);
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
  int mday = localtime_->tm_mday;
  while (true) {
    if (updateday(mday)) {
      mday = localtime_->tm_mday;
      std::strftime(dateBuffer, sizeof(dateBuffer), "%Y.%m.%d", localtime_);
      date = dateBuffer;
      logger_.close();
      logger_.open(filenameSuffix_ + date, std::ios::app);
    }
    blockqueue_.pop_front(msg);
    logger_ << msg << std::endl;
  }
}

bool updateday(int mday) {
  auto now_ = std::chrono::system_clock::now();
  std::time_t time_ = std::chrono::system_clock::to_time_t(now_);
  return std::localtime(&time_)->tm_mday != mday;
}