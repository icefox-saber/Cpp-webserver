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

/// @brief 任务线程池
class threadPool {
private:
  /// @brief 结束标志
  bool stop;
  /// @brief 任务队列
  std::queue<std::function<void()>> tasks;
  /// @brief 线程
  std::vector<std::thread> threads;
  /// @brief 条件变量
  std::condition_variable cv;
  /// @brief 任务队列线程安全的锁
  std::mutex task_mtx;
  // std::mutex cout_mtx;调试用

public:
  /// @brief 线程池
  /// @param num 线程数量
  threadPool(size_t num = 1);
  /// @brief 添加任务到任务队列中
  /// @tparam F 任务的函数类型
  /// @tparam ...Arg 任务函数的变长参数类型
  /// @param f 函数
  /// @param ...arg 函数的参数
  template <typename F, typename... Arg> void emplace(F &&f, Arg &&...arg);
  /// @brief 结束接受任务，并且将任务队列中的任务全部完成后销毁
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