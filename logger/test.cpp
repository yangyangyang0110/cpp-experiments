/*
 * @Author: yangzilong
 * @Date: 2021-12-20 16:21:31
 * @Last Modified by: yangzilong
 * @Email: yangzilong@objecteye.com
 * @Description:
 */
#include "helper/logger.hpp"
#include <future>
#include <iostream>
#include <spdlog/common.h>
#include <utility>

using namespace helper::logger;

template<typename Func, typename... Args>
void function_wrapper(Func &&func, Args &&...args) {
  try {
    std::forward<Func>(func)(std::forward<Args>(args)...);
  } catch (const spdlog::spdlog_ex &e) { printf(">>> catch exception, error raeson: %s.\n", e.what()); }
}

void test_multi_logger(const int num_repeat) {
  const auto log_max_size_mb = 10;
  const auto log_base_path = "../../log/log";
  const auto log_max_keep_days = 10;

  init_multi_level_files_logger_thread_safe(LogLevel::TRACE, "logger", log_base_path, log_max_size_mb,
                                            log_max_keep_days);

  for (int i = 0; i < num_repeat; ++i) {
    LOG_TRACE("trace log {} {}", i, "yyyy");
    LOG_DEBUG("debug log {} {}", i, "yyyy");
    LOG_INFO("info log {} {}", i, "yyyy");
    LOG_WARN("warn log {} {}", i, "yyyy");
    LOG_ERROR("error log {} {}", i, "yyyy");
    LOG_CRITICAL("fatal log {} {}", i, "yyyy");
  }
}


void test_multi_logger_concurrency() {
  constexpr int num_thread = 10;
  std::promise<void> hang;
  std::vector<std::thread> threads(num_thread);
  std::shared_future<void> sf = hang.get_future().share();
  threads.shrink_to_fit();

  for (std::remove_cv<decltype(num_thread)>::type i = 0; i < num_thread; ++i) {
    threads.at(i) = std::thread([sf]() {
      sf.wait();
      std::cout << ">>> thread [" << std::this_thread::get_id() << "] start." << std::endl;
      test_multi_logger(10);
      std::cout << ">>> thread [" << std::this_thread::get_id() << "] end." << std::endl;
    });
  }

  hang.set_value();

  for (auto &t: threads)
    t.join();
}

int main() {
#ifdef MyDebug
  printf("Debug Mode.\n");
#endif
  // function_wrapper(test_combined_sinks);
  // function_wrapper(test_multi_logger_split_by_level);
  function_wrapper(test_multi_logger, 1);
  function_wrapper(test_multi_logger_concurrency);
  return 0;
}
