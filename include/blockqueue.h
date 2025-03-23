#ifndef BLOCK_QUEUE_H
#define BLOCK_QUEUE_H

#include <condition_variable>
#include <deque>
#include <mutex>
#include <queue>

/// @brief 线程安全的生产者消费者队列
/// @tparam T 队列中存储的数据类型
template <typename T> class blockqueue {
private:
  /// @brief 块队列的最大长度
  std::size_t capacity_;
  /// @brief 生产者条件变量
  std::condition_variable productor_;
  /// @brief 消费者条件变量
  std::condition_variable consumer_;
  /// @brief 互斥锁
  std::mutex mtx_;
  /// @brief 数据队列
  std::deque<T> deque_;

public:
  /// @brief 初始化队列
  /// @param capacity 队列允许的最大储存长度
  blockqueue(std::size_t capacity);
  /// @brief 添加元素到队列末
  /// @param msg 添加到队列的元素
  void push_back(T &&msg);
  /// @brief 添加元素到队列首
  /// @param msg 添加到队列的元素
  void push_front(T &&msg);
  /// @brief 从队列首取出一个元素，取完该元素会消失
  /// @param dest 存储取出元素的变量
  void pop_front(T &dest);
  ~blockqueue() {};
};
template <typename T>
blockqueue<T>::blockqueue(std::size_t capacity) : capacity_(capacity) {}

template <typename T> void blockqueue<T>::push_back(T &&msg) {
  {
    std::unique_lock lck(mtx_);
    productor_.wait(lck, [this]() { return deque_.size() < capacity_; });
    deque_.push_back(std::forward<T>(msg));
  }
  consumer_.notify_one();
}

template <typename T> void blockqueue<T>::push_front(T &&msg) {
  {
    std::unique_lock lck(mtx_);
    productor_.wait(lck, [this]() { return deque_.size() < capacity_; });
    deque_.push_front(std::forward(msg));
  }
  consumer_.notify_one();
}

template <typename T> void blockqueue<T>::pop_front(T &msg) {
  {
    std::unique_lock lck(mtx_);
    consumer_.wait(lck, [this]() { return !deque_.empty(); });
    msg = std::move(deque_.front());
    deque_.pop_front();
  }
  productor_.notify_one();
}
#endif
