#include "../include/log.h"

logger::logger(std::string file_)
    : blockqueue_(10), logger_(file_, std::ios::app),
      writethread_(&logger::write, this) {}

logger &logger::instance() {
  static logger inst_;
  return inst_;
}

logger::~logger() {}

void logger::log(std::string str) { blockqueue_.push_back(std::move(str)); }

void logger::write() {
  std::string msg;
  while (true) {
    blockqueue_.pop_front(msg);
    logger_ << msg << std::endl;
  }
}