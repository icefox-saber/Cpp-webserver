#include "../include/log.h"
#include <thread>
int main() {
  std::thread t1([]() {
    for (size_t i = 0; i < 1000; i++) {
      logger::instance()->log("text1");
    }
  });
  std::thread t2([]() {
    for (size_t i = 0; i < 1000; i++) {
      logger::instance()->log("text2");
    }
  });
  t1.join();
  t2.join();
  return 0;
}