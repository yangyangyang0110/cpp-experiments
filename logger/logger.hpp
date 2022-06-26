/*
* File: logger.hpp
* Created Date: 2022/6/25
* Author: yangzilong (yangyangyang0110@outlook.com)
* Description:
* -----
* Last Modified: 2022/6/25 19:34:56
* Modified By: yang (yangyangyang0110@outlook.com)
* -----
* Copyright (c) 2022  . All rights reserved.
*/

#pragma once

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <spdlog/common.h>
#include <spdlog/details/circular_q.h>
#include <spdlog/details/file_helper.h>
#include <spdlog/details/null_mutex.h>
#include <spdlog/details/os.h>
#include <spdlog/details/synchronous_factory.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <cstdio>
#include <ctime>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <vector>


/**
 *  use fmt lib, e.g. LOG_WARN("warn log, {1}, {1}, {2}", 1, 2);
 */

#define FILENAME_ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)


namespace spdlog {
namespace sinks {
template<typename Mutex>
class easy_file_sink final : public base_sink<Mutex> {
public:
  easy_file_sink(filename_t base_filename, size_t max_size, size_t max_keep_days = 0)
      : base_filename_(std::move(base_filename)), max_size_(max_size), max_keep_days_(max_keep_days) {
    auto now = log_clock::now();
    auto filename = gen_filename_by_daily(base_filename_, now_tm(now));

    file_helper_.open(filename, false);
    current_size_ = file_helper_.size();
    rotation_tp_ = next_rotation_tp_();

    if (max_keep_days_ > 0) {
      file_path_list_.push_back(std::move(std::set<filename_t>()));
      file_path_list_[file_path_list_.size() - 1].insert(filename);
    }
  }

  filename_t filename() {
    std::lock_guard<Mutex> lock(base_sink<Mutex>::mutex_);
    return file_helper_.filename();
  }

protected:
  void sink_it_(const details::log_msg &msg) override {
    memory_buf_t formatted;
    base_sink<Mutex>::formatter_->format(msg, formatted);
    current_size_ += formatted.size();

    auto time = msg.time;
    if (time >= rotation_tp_) {
      file_helper_.close();
      auto filename = gen_filename_by_daily(base_filename_, now_tm(time));
      file_helper_.open(filename, false);
      current_size_ = file_helper_.size();
      rotation_tp_ = next_rotation_tp_();

      {
        file_path_list_.push_back(std::move(std::set<filename_t>()));
        file_path_list_[file_path_list_.size() - 1].emplace(filename);
      }

      // Do the cleaning only at the end because it might throw on failure.
      if (max_keep_days_ > 0 && file_path_list_.size() > max_keep_days_)
        delete_old_();
    } else if (current_size_ >= max_size_) {
      file_helper_.close();
      auto src_name = gen_filename_by_daily(base_filename_, now_tm(time));
      auto target_name =
          gen_filename_by_filesize(base_filename_, now_tm(time), file_path_list_[file_path_list_.size() - 1].size());

      // rename file if failed then us `target_name` as src_name.
      if (!rename_file_(src_name, target_name)) {
        details::os::sleep_for_millis(200);
        if (!rename_file_(src_name, target_name)) {
          fprintf(stderr, "%s:%d rename %s to %s failed\n", FILENAME_, __LINE__, src_name.c_str(), target_name.c_str());
          src_name = target_name;
        }
      }

      file_path_list_[file_path_list_.size() - 1].emplace(src_name);
      if (src_name != target_name)
        file_path_list_[file_path_list_.size() - 1].emplace(target_name);

      file_helper_.open(src_name, false);
      current_size_ = file_helper_.size();
      rotation_tp_ = next_rotation_tp_();
    }

    file_helper_.write(formatted);
  }

  void flush_() override { file_helper_.flush(); }

private:
  tm now_tm(log_clock::time_point tp) {
    time_t t_now = log_clock::to_time_t(tp);
    return spdlog::details::os::localtime(t_now);
  }

  /**
   * @brief Get next day tm.
   *
   * @return log_clock::time_point
   */
  log_clock::time_point next_rotation_tp_() {
    auto now = log_clock::now();
    tm date = now_tm(now);
    date.tm_hour = 0;
    date.tm_min = 0;
    date.tm_sec = 0;
    auto rotation_time = log_clock::from_time_t(std::mktime(&date));
    if (rotation_time > now)
      return rotation_time;
    return {rotation_time + std::chrono::hours(24)};
  }

  // Delete the file N rotations ago.
  // Throw spdlog_ex on failure to delete the old file.
  void delete_old_() {
    for (auto iter = file_path_list_.begin(); iter != file_path_list_.end();) {
      if (file_path_list_.size() <= max_keep_days_)
        break;

      for (auto it = iter->begin(); it != iter->end(); ++it) {
        bool ok = details::os::remove_if_exists(*it) == 0;
        if (!ok) {
          fprintf(stderr, "%s.\n",
                  fmt::format("Failed removing daily file {}", details::os::filename_to_str(*it)).c_str());
        }
        file_path_list_.erase(iter);
      }
    }
  }
  /*  */
  static filename_t gen_filename_by_daily(const filename_t &filename, const tm &now_tm) {
    filename_t basename, ext;
    std::tie(basename, ext) = details::file_helper::split_by_extension(filename);
    return fmt::format(SPDLOG_FILENAME_T("{}-{:04d}{:02d}{:02d}{}"), basename, now_tm.tm_year + 1900, now_tm.tm_mon + 1,
                       now_tm.tm_mday, ext);
  }

  //
  static filename_t gen_filename_by_filesize(const filename_t &filename, const tm &now_tm, const int &idx) {
    filename_t basename, ext;
    std::tie(basename, ext) = details::file_helper::split_by_extension(filename);
    return fmt::format(SPDLOG_FILENAME_T("{}-{:04d}{:02d}{:02d}-{:02d}{:02d}{:02d}.{:d}{}"), basename,
                       now_tm.tm_year + 1900, now_tm.tm_mon + 1, now_tm.tm_mday, now_tm.tm_hour, now_tm.tm_min,
                       now_tm.tm_sec, idx, ext);
  }

  static bool rename_file_(const filename_t &src_filename, const filename_t &target_filename) {
    (void) details::os::remove(target_filename);
    return details::os::rename(src_filename, target_filename) == 0;
  }

  filename_t base_filename_;
  log_clock::time_point rotation_tp_;
  details::file_helper file_helper_;
  std::size_t max_size_;
  std::size_t max_keep_days_;
  std::size_t current_size_;
  std::vector<std::set<filename_t>> file_path_list_;
};

using easy_file_sink_mt = easy_file_sink<std::mutex>;
using easy_file_sink_st = easy_file_sink<details::null_mutex>;

} // namespace sinks

template<typename Factory = spdlog::synchronous_factory>
inline std::shared_ptr<logger> easy_logger_mt(std::string logger_name, const filename_t &filename, size_t max_size,
                                              size_t max_keep_days = -1) {
  return Factory::template create<sinks::easy_file_sink_mt>(std::move(logger_name), filename, max_size, max_keep_days);
}

template<typename Factory = spdlog::synchronous_factory>
inline std::shared_ptr<logger> easy_logger_st(const std::string &logger_name, const filename_t &filename,
                                              size_t max_size, size_t max_keep_days = -1) {
  return Factory::template create<sinks::easy_file_sink_st>(logger_name, filename, max_size, max_keep_days);
}

} // namespace spdlog


#define LOG_TRACE(...)                                                                                                 \
  (g_logger_trace_ptr) ? SPDLOG_LOGGER_TRACE(g_logger_trace_ptr, __VA_ARGS__) : SPDLOG_TRACE(__VA_ARGS__)
#define LOG_DEBUG(...)                                                                                                 \
  (g_logger_debug_ptr) ? SPDLOG_LOGGER_DEBUG(g_logger_debug_ptr, __VA_ARGS__) : SPDLOG_DEBUG(__VA_ARGS__)
#define LOG_INFO(...)                                                                                                  \
  (g_logger_info_ptr) ? SPDLOG_LOGGER_INFO(g_logger_info_ptr, __VA_ARGS__) : SPDLOG_INFO(__VA_ARGS__)
#define LOG_WARN(...)                                                                                                  \
  (g_logger_warn_ptr) ? SPDLOG_LOGGER_WARN(g_logger_warn_ptr, __VA_ARGS__) : SPDLOG_WARN(__VA_ARGS__)
#define LOG_ERROR(...)                                                                                                 \
  (g_logger_error_ptr) ? SPDLOG_LOGGER_ERROR(g_logger_error_ptr, __VA_ARGS__) : SPDLOG_ERROR(__VA_ARGS__)
#define LOG_CRITICAL(...)                                                                                              \
  (g_logger_critical_ptr) ? SPDLOG_LOGGER_CRITICAL(g_logger_critical_ptr, __VA_ARGS__) : SPDLOG_CRITICAL(__VA_ARGS__)

enum class LogLevel {
  CLOSE = -1,
  TRACE = 0,
  DEBUG = 1,
  INFO = 2,
  WARN = 3,
  ERROR = 4,
  CRITICAL = 5,
};


/**
 * @brief 使用global pointer实现不同level日志写入不同文件的功能
 */

static std::shared_ptr<spdlog::logger> g_logger_trace_ptr{nullptr}, g_logger_debug_ptr{nullptr},
    g_logger_info_ptr{nullptr}, g_logger_warn_ptr{nullptr}, g_logger_error_ptr{nullptr}, g_logger_critical_ptr{nullptr};


#if __cplusplus >= 201402L
static std::map<LogLevel, spdlog::string_view_t> g_log_level_to_string;
#else
static std::map<LogLevel, const char *> g_log_level_to_string;
#endif


namespace details {

static void init_global_static_variables() {
  ::g_log_level_to_string = {
      {LogLevel::CLOSE, "close"},       {LogLevel::TRACE, "trace"}, {LogLevel::DEBUG, "debug"},
      {LogLevel::INFO, "info"},         {LogLevel::WARN, "warn"},   {LogLevel::ERROR, "error"},
      {LogLevel::CRITICAL, "critical"},
  };
}


inline spdlog::level::level_enum to_spd_level(const LogLevel &level) {
  switch (level) {
    case LogLevel::TRACE:
      return spdlog::level::trace;
    case LogLevel::DEBUG:
      return spdlog::level::debug;
    case LogLevel::INFO:
      return spdlog::level::info;
    case LogLevel::WARN:
      return spdlog::level::warn;
    case LogLevel::ERROR:
      return spdlog::level::err;
    case LogLevel::CRITICAL:
      return spdlog::level::critical;
    case LogLevel::CLOSE:
      return spdlog::level::off;
    default:
      return spdlog::level::info;
  }
}

std::vector<LogLevel> find_high_level_logger(const LogLevel &level) {
  std::vector<LogLevel> res;
  res.reserve(g_log_level_to_string.size());
  const int level_int = int(level);
  for (const auto &iter: g_log_level_to_string) {
    if (int(iter.first) >= int(level_int) && iter.first != LogLevel::CLOSE) {
      res.emplace_back(iter.first);
    }
  }
  res.shrink_to_fit();
  return res;
}

inline std::shared_ptr<spdlog::logger> make_shared_logger(const LogLevel &level, std::string &&logger_name,
                                                          std::string &&file_path, size_t max_file_size_bytes,
                                                          size_t max_keep_days) noexcept(false) {
  if (max_file_size_bytes <= 0 || max_keep_days <= 0) {
    throw spdlog::spdlog_ex("`max_file_size_bytes` and `max_keep_days` must greater than zero !!!");
  }

  spdlog::level::level_enum spd_level = details::to_spd_level(level);
  std::vector<spdlog::sink_ptr> sinks{
      std::make_shared<spdlog::sinks::easy_file_sink_mt>(std::move(file_path), max_file_size_bytes, max_keep_days),
  };

  if (level >= LogLevel::WARN) {
    sinks.reserve(2);
    sinks.push_back(std::make_shared<spdlog::sinks::stderr_color_sink_mt>());
  }
#if 1
  else if (level == LogLevel::INFO) {
    sinks.reserve(2);
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
  }
#endif
  sinks.shrink_to_fit();

  auto logger = std::make_shared<spdlog::logger>(std::move(logger_name), std::begin(sinks), std::end(sinks));
  logger->set_level(spd_level);
  logger->set_pattern("[%L %C-%m-%d %T.%e %P %t %!] [%s:%#] %v");
  return logger;
}

inline void set_logger_native(std::shared_ptr<spdlog::logger> &logger, std::shared_ptr<spdlog::logger> &&ptr) {
  if (!logger) {
    logger = std::move(ptr);
  }
}

inline void set_global_logger_pointer_by_log_level(const LogLevel &level, std::shared_ptr<spdlog::logger> &&ptr) {
  switch (level) {
    case LogLevel::TRACE: {
      set_logger_native(g_logger_trace_ptr, std::move(ptr));
    } break;
    case LogLevel::DEBUG: {
      set_logger_native(g_logger_debug_ptr, std::move(ptr));
    } break;
    case LogLevel::INFO: {
      set_logger_native(g_logger_info_ptr, std::move(ptr));
    } break;
    case LogLevel::WARN: {
      set_logger_native(g_logger_warn_ptr, std::move(ptr));
    } break;
    case LogLevel::ERROR: {
      set_logger_native(g_logger_error_ptr, std::move(ptr));
    } break;
    case LogLevel::CRITICAL: {
      set_logger_native(g_logger_critical_ptr, std::move(ptr));
    } break;
    default: {
    } break;
  }
}

inline void set_multi_level_files_logger(const LogLevel &log_level, const std::string &logger_basename,
                                         const std::string &log_base_path_wo_suffix, size_t max_file_size_bytes,
                                         size_t max_keep_days) noexcept(false) {
  if (max_file_size_bytes <= 0 || max_keep_days <= 0) {
    throw spdlog::spdlog_ex("`max_file_size_bytes` and `max_keep_days` must greater than zero !!!");
  }

  fmt::print(">>> set_multi_level_files_logger called.\n");

  init_global_static_variables();

  spdlog::set_level(details::to_spd_level(log_level));

  const auto log_level_list = details::find_high_level_logger(log_level);

  for (const auto &level: log_level_list) {
    const auto log_level_str = g_log_level_to_string.at(level);
    auto log_name = fmt::format("{}-{}", logger_basename, log_level_str);
    auto log_path = fmt::format("{}.{}", log_base_path_wo_suffix, log_level_str);
    auto logger = details::make_shared_logger(level, std::move(log_name), std::move(log_path), max_file_size_bytes,
                                              max_keep_days);
    details::set_global_logger_pointer_by_log_level(level, std::move(logger));
  }
}

} // namespace details

inline void init_multi_level_files_logger_thread_safe(const LogLevel &log_level, const std::string &logger_basename,
                                                     const std::string &log_base_path_wo_suffix,
                                                     size_t max_file_size_bytes, size_t max_keep_days) {
  static std::once_flag once_flag;
  std::call_once(once_flag, details::set_multi_level_files_logger, log_level, logger_basename, log_base_path_wo_suffix,
                 max_file_size_bytes, max_keep_days);
}
