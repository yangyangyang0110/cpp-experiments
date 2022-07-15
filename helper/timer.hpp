/* Proj: experiments
 * File: timer.hpp
 * Created Date: 2022/7/8
 * Author: yangzilong (yangyangyang0110@outlook.com)
 * Description: 
 * -----
 * Last Modified: 2022/7/8 00:23:51
 * Modified By: yang (yangyangyang0110@outlook.com)
 * -----
 * Copyright (c) 2022  . All rights reserved.
 */
#ifndef EXPERIMENTS_TIMER_HPP
#define EXPERIMENTS_TIMER_HPP
#include <chrono>


#define TIMER_START(name) helper::timer::Timer timer_##name(true)
#define TIMER_END(name) printf("[%s] time cost %lu(ms).\n", #name, timer_##name.GetTimeCost())


#define TIMER_START_RENDER(name) helper::timer::Timer timer_name(true)
#define TIMER_END_RENDER(name) printf("[%s] time cost %lu(ms).\n", name, timer_name.GetTimeCost())

#define TIMER_NS_START_RENDER(name) helper::timer::Timer timer_ns_name(true)
#define TIMER_NS_END_RENDER(name)                                                                                      \
  printf("[%s] time cost %lu(ns).\n", name, timer_ns_name.GetTimeCost<std::chrono::nanoseconds>())


namespace helper::timer {

class Timer {
  std::chrono::high_resolution_clock::time_point start_time_point_;

public:
  explicit Timer(bool start = false) {
    if (start)
      Reset();
  }

  void Reset() noexcept { start_time_point_ = std::chrono::high_resolution_clock::now(); }

  template<typename T = std::chrono::milliseconds>
  auto GetTimeCost() const noexcept {
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<T>(now - start_time_point_).count();
  }
};
} // namespace helper::timer


#endif //EXPERIMENTS_TIMER_HPP
