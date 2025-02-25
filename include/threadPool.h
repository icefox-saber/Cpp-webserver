#ifndef THREADPOOL_H
#define THREADPOOL_H

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
  // std::mutex cout_mtx;调试用

public:
  threadPool(size_t num = 1);
  template <typename F, typename... Arg> void emplace(F &&f, Arg &&...arg);
  ~threadPool();
};

template <typename F, typename... Arg>
void threadPool::emplace(F &&f, Arg &&...arg) {
  std::function<void()> task =
      std::bind(std::forward<F>(f), std::forward<Arg>(arg)...);
  {
    std::unique_lock task_lk(task_mtx);
    tasks.emplace(std::move(task));
  }

  cv.notify_one();
}

#endif