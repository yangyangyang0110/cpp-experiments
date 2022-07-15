/* Proj: experiments
 * File: buffer-available.cpp
 * Created Date: 2022/7/12
 * Author: yangzilong (yangyangyang0110@outlook.com)
 * Description: 
 * -----
 * Last Modified: 2022/7/12 14:43:28
 * Modified By: yang (yangyangyang0110@outlook.com)
 * -----
 * Copyright (c) 2022  . All rights reserved.
 */

#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <type_traits>

#include <vector>

static constexpr size_t MAX_QUEUE_SIZE = 10;

template<typename T>
  requires std::is_nothrow_move_assignable_v<T>
class ThreadSafeQueueCapacity {
  std::queue<T> buffer_;
  mutable std::mutex mutex_, mutex_for_max_size_;
  std::condition_variable cv_, cv_for_max_size_;

public:
  void WaitAndPush(T &&t) { WaitAndEmplace(std::move(t)); }

  void WaitAndPush(T const &t) { WaitAndEmplace(t); }

  template<typename... Args>
    requires(std::is_constructible_v<Args..., T>)
  void WaitAndEmplace(Args &&...args) {
    {
      std::unique_lock<std::mutex> lock(mutex_for_max_size_);
      cv_for_max_size_.wait(lock, [this]() { return size() < MAX_QUEUE_SIZE; });
    }
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
    cv_for_max_size_.notify_one();
    return true;
  }

  T WaitAndPop() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this]() { return !buffer_.empty(); });
    T res = std::move(buffer_.back());
    buffer_.pop();
    cv_for_max_size_.notify_one();
    return res;
  }

  bool empty() const noexcept { return size() == 0; }

  size_t size() const noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    return buffer_.size();
  }
};


void test_buffer_with_max_size() {
  ThreadSafeQueueCapacity<int> queue;

  std::promise<void> promise;
  std::shared_future<void> sf = promise.get_future().share();

  std::thread t1([&]() {
    sf.wait();
    for (int i = 0; i < 30; ++i) {
      queue.WaitAndPush(i);
      printf("[%d] ping.\n", i);
    }
  });

  std::thread t2([&]() {
    sf.wait();
    for (int i = 0; i < 10; ++i) {
      queue.WaitAndPop();
      printf("[%d] pong.\n", i);
    }
  });

  promise.set_value();

  t1.join();
  // t2.join();

  printf("Done. Queue Size: %lu.\n", queue.size());

}

int main() {
  test_buffer_with_max_size();
  return 0;
}
