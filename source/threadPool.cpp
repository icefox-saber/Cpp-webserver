#include "../include/threadPool.h"

threadPool::threadPool(size_t num) {
  {
    std::unique_lock lk(task_mtx);
    stop = false;
  }
  for (size_t i = 0; i < num; i++) {
    threads.emplace_back([this]() {
      std::function<void()> task;
      while (true) {
        {
          std::unique_lock task_lk(task_mtx);
          cv.wait(task_lk, [this]() { return !tasks.empty() || stop; });

          if (stop && tasks.empty()) {
            return;
          }

          task = std::move(tasks.front());
          tasks.pop();
        } // lk作用区
        // std::unique_lock cout_lk(cout_mtx);调试用
        task();
      }
    });
  }
}

threadPool::~threadPool() {
  {
    std::unique_lock task_lk(task_mtx);
    stop = true;
  }
  cv.notify_all();
  for (auto &i : threads) {
    if (i.joinable()) {
      i.join();
    }
  }
}
