/* Proj: experiments
 * File: buffer.hpp
 * Created Date: 2022/7/5
 * Author: yangzilong (yangyangyang0110@outlook.com)
 * Description: 
 * -----
 * Last Modified: 2022/7/5 22:11:37
 * Modified By: yang (yangyangyang0110@outlook.com)
 * -----
 * Copyright (c) 2022  . All rights reserved.
 */
#ifndef EXPERIMENTS_BUFFER_HPP
#define EXPERIMENTS_BUFFER_HPP


#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

namespace helper::buffer {

template<typename T>
  requires std::is_nothrow_copy_assignable_v<T> && std::is_nothrow_move_assignable_v<T>
class ThreadSafeQueue {
  std::queue<T> buffer_;
  mutable std::mutex mutex_;
  std::condition_variable cv_;

public:
  void Push(T &&t) {
    std::lock_guard<std::mutex> lock(mutex_);
    buffer_.push(std::move(t));
    cv_.notify_all();
  }

  void Push(T const &t) {
    std::lock_guard<std::mutex> lock(mutex_);
    buffer_.push(t);
    cv_.notify_all();
  }

  template<typename... Args>
    requires(std::is_constructible_v<Args..., T>)
  void Emplace(Args &&...args) {
    std::lock_guard<std::mutex> lock(mutex_);
    buffer_.push(std::forward<Args>(args)...);
    cv_.notify_all();
  }

  bool TryPop(T &res) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (buffer_.empty())
      return false;
    res = std::move(buffer_.back());
    buffer_.pop();
    return true;
  }

  T WaitAndPop() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this]() { return !buffer_.empty(); });
    T res = std::move(buffer_.back());
    buffer_.pop();
    return res;
  }

  bool empty() const noexcept {
    return size() == 0;
  }

  size_t size() const noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    return buffer_.size();
  }
};

template<typename T>
using LockBaseQueue = ThreadSafeQueue<T>;

} // namespace helper::buffer


#endif //EXPERIMENTS_BUFFER_HPP
