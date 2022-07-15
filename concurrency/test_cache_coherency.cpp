/* Proj: experiments
 * File: test_cache_coherency.cpp
 * Created Date: 2022/7/8
 * Author: yangzilong (yangyangyang0110@outlook.com)
 * Description: 
 * -----
 * Last Modified: 2022/7/8 00:13:44
 * Modified By: yang (yangyangyang0110@outlook.com)
 * -----
 * Copyright (c) 2022  . All rights reserved.
 */
#include "helper/timer.hpp"
#include <algorithm>
#include <cstddef>
#include <ranges>
#include <thread>

static constexpr size_t COUNT = 10 * 1000 * 1000;

/**
 * @brief TODO: 使用align_of是否可以达到同样的效果, 因为主要是靠结构体大小减少了缓存命中性, 所以减少了缓存一致性的bug.
 *  (因为默认OS一次去内存取64字节数据, 会加载到缓存中, 然后`TrueFoo`数组, 在两个不同的cpu的l1或l2缓存中, 频繁修改导致缓存一致性开销很大);
 *  ()
 */

struct TrueFoo {
  long long x;
};


struct FalseFoo {
  long long x0, x1, x2, x3, x4, x5, x6, x7;
  long long x;
  long long x8, x9, x10, x11, x12, x13, x14, x15;
};

template<typename T, size_t NumberArray = 2>
void test_core(char const *description) {
  T foo_arr[NumberArray];
  TIMER_NS_START_RENDER(description);
  std::thread t1([&]() {
    for (long long i = 0; i < COUNT; ++i) {
      foo_arr[0].x = i;
    }
  });
  std::thread t2([&]() {
    for (long long i = 0; i < COUNT; ++i) {
      foo_arr[1].x = i;
    }
  });
  t1.join();
  t2.join();
  TIMER_NS_END_RENDER(description);
}

void test() {
  test_core<FalseFoo>("no cache coherency");
  test_core<TrueFoo>("has cache coherency");
}

int main() {
  test();
  return 0;
}
