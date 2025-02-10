#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

std::mutex coutmtx, cerrmtx;

int main() {
  int i = 2;
  int j = (0x010 & 0x001);
  std::cout << j;
  std::string s;
  s.length();
  auto k = s.begin() - 2;
  auto l = *k;
  auto m = s.begin() - k;
  return 0;
}