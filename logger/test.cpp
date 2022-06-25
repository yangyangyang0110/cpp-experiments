/*
 * @Author: yangzilong
 * @Date: 2021-12-20 16:21:31
 * @Last Modified by: yangzilong
 * @Email: yangzilong@objecteye.com
 * @Description:
 */
#include "./logger.hpp"
#include "spdlog/logger.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <future>
#include <iostream>
#include <spdlog/common.h>
#include <utility>

template<typename Func, typename... Args>
void function_wrapper(Func &&func, Args &&...args) {
  try {
    std::forward<Func>(func)(std::forward<Args>(args)...);
  } catch (const spdlog::spdlog_ex &e) { printf(">>> catch exception, error raeson: %s.\n", e.what()); }
}

void test_combined_sinks() {
  auto log_path = "./log/log.log";
  auto max_size_bytes = 100;
  auto max_files = 2;
  std::vector<spdlog::sink_ptr> sinks{
      std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_path, max_size_bytes, max_files),
      std::make_shared<spdlog::sinks::daily_file_sink_mt>(log_path, 23, 59)};

  spdlog::logger logger("combined_logger", std::begin(sinks), std::end(sinks));
  logger.set_level(spdlog::level::trace);

  logger.trace("trace log");
  logger.debug("debug log");
  logger.info("info log");
  logger.warn("warn log");
  logger.error("error log");
}

void test_color_sink() {
  auto console_sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
  console_sink->set_level(spdlog::level::debug);
  console_sink->set_pattern("[color_sink_example] [%^%l%$] %v");

  spdlog::logger logger("color-sink", {console_sink});
  logger.debug("debug log");
  logger.info("info log");
  logger.warn("warn log");
  logger.error("error log");
}

void test_multi_logger(const int num_repeat) {
  const auto log_max_size_bytes = 10 * 1024 * 1024;
  const auto log_base_path = "../../log/log";
  const auto log_max_keep_days = 10;

  set_multi_level_files_logger_thread_safe(LogLevel::TRACE, "logger", log_base_path, log_max_size_bytes,
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
