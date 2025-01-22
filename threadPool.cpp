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
  std::mutex mtx;

public:
  threadPool(int num);
  template <typename F, typename... Arg> void emplace(F &&f, Arg &&...arg);
  ~threadPool();
};

template <typename F, typename... Arg>
void threadPool::emplace(F &&f, Arg &&...arg) {
  std::function<void()> task =
      std::bind(std::forward<F>(f), std::forward<Arg>(arg)...);
  {
    std::unique_lock lock(mtx);
    tasks.emplace(task);
  }

  cv.notify_one();
}

threadPool::threadPool(int num) {
  {
    std::unique_lock lk(mtx);
    stop = false;
  }
  for (size_t i = 0; i < num; i++) {
    threads.emplace_back([this]() {
      std::function<void()> task;
      while (true) {
        {
          std::unique_lock lk(mtx);
          cv.wait(lk, [this]() { return !tasks.empty() || stop; });

          if (stop && tasks.empty()) {
            return;
          }

          task = std::move(tasks.front());
          tasks.pop();
        } // lk作用区

        task();
      }
    });
  }
}

threadPool::~threadPool() {
  {
    std::unique_lock lk(mtx);
    stop = true;
  }
  cv.notify_all();
  for (auto &i : threads) {
    if (i.joinable()) {
      i.join();
    }
  }
}

int main() {
  threadPool tp(4);
  for (size_t i = 0; i < 10; i++) {
    std::cout << "task " << i << "in" << std::endl;
    tp.emplace(
        [](int num) { std::cout << "task " << num << "out" << std::endl; }, i);
  }

  return 0;
}