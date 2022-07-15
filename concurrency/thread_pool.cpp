/* Proj: experiments
 * File: thread_pool.cpp
 * Created Date: 2022/7/6
 * Author: yangzilong (yangyangyang0110@outlook.com)
 * Description: 
 * -----
 * Last Modified: 2022/7/6 22:54:38
 * Modified By: yang (yangyangyang0110@outlook.com)
 * -----
 * Copyright (c) 2022  . All rights reserved.
 */

/**
 * @brief TODO:
 */
#include "helper/logger.hpp"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>

template<int N>
class ThreadPool {
  std::array<std::thread, N> threads_;
  std::queue<std::function<void(void)>> task_queue_;
  mutable std::mutex mutex_;
  mutable std::condition_variable cv_;
  std::atomic<bool> is_run_;
  std::atomic<int32_t> action_thread_;

public:
  ~ThreadPool() {
    is_run_.store(false, std::memory_order_relaxed);
    cv_.notify_all();
    for (auto &t: threads_)
      t.join();
  }

  ThreadPool() : action_thread_(0) {
    is_run_.store(true, std::memory_order_relaxed);
    int thread_id = 0;
    for (auto &t: threads_) {
      t = std::thread(
          [this](std::string const &thread_name) {
            while (true) {
              std::function<void(void)> task;
              {
                std::unique_lock lock(mutex_);
                cv_.wait(lock, [this]() { return !is_run_.load(std::memory_order_relaxed) || !task_queue_.empty(); });
                if (!is_run_.load(std::memory_order_relaxed) && task_queue_.empty())
                  return;
                task = std::move(task_queue_.back());
                task_queue_.pop();
              }

              ++action_thread_;
              LOG_DEBUG("[{}] get task.", thread_name);
              task();
              LOG_DEBUG("[{}] call task.", thread_name);
              --action_thread_;
            }
          },
          fmt::format("thread_id-{}", thread_id++));
    }
  }

  template<typename Func, typename... Args, typename RetType = typename std::result_of<Func(Args...)>::type>
  std::future<RetType> Execute(Func &&func, Args &&...args) {
    std::packaged_task<RetType(void)> task(std::bind(std::forward<Func>(func), std::forward<Args>(args)...));
    auto fut = task.get_future();

    auto task_ptr = std::make_shared<decltype(task)>(std::move(task));
    auto task_wrap = [task_ptr]() { task_ptr->operator()(); };

    std::lock_guard lock(mutex_);
    task_queue_.emplace(std::move(task_wrap));
    cv_.notify_all();
    return fut;
  }
};

void test() {
  constexpr int THREAD_NUMBER = 3;
  constexpr size_t TASK_NUMBER = 10;
  ThreadPool<THREAD_NUMBER> thread_pool;
  std::vector<std::future<int>> futures(TASK_NUMBER);

  for (size_t i = 0; i < TASK_NUMBER; ++i) {
    futures.at(i) = thread_pool.Execute([](int i) { return i * 10; }, i);
  }

  std::this_thread::sleep_for(std::chrono::seconds(5));

  for (auto &f: futures) {
    LOG_INFO(">>> fut result: {}", f.get());
  }
}

int main() {
  auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  auto logger = std::make_shared<spdlog::logger>(FILENAME_, sink);
  logger->set_pattern("[%L %Y/%m/%d %T.%e (P)%P (T)%t] [%!] [%s:%#] %v");
  logger->set_level(spdlog::level::debug);
  spdlog::set_default_logger(logger);

  test();
  return 0;
}