#ifndef BLOCK_QUEUE_H
#define BLOCK_QUEUE_H

#include <condition_variable>
#include <deque>
#include <mutex>
#include <queue>

template <typename T> class blockqueue {
private:
  std::size_t capacity_;
  std::condition_variable productor_;
  std::condition_variable consumer_;
  std::mutex mtx_;
  std::deque<T> deque_;

public:
  blockqueue(std::size_t capacity);
  void push_back(T &&msg);
  void push_front(T &&msg);
  void pop_front(T &dest);
  ~blockqueue(){};
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
