#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

class threadPool {
private:
  bool stop;
  std::queue<std::function<void()>> tasks;
  std::vector<std::thread> threads;
  std::condition_variable cv;
  std::mutex task_mtx;
  std::mutex cout_mtx;
public:
  threadPool(size_t num);
  template <typename F, typename... Arg> void emplace(F &&f, Arg &&...arg);
  ~threadPool();
};