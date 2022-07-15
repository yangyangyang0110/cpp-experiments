/* Proj: experiments
 * File: recursive_tools.cpp
 * Created Date: 2022/7/2
 * Author: yangzilong (yangyangyang0110@outlook.com)
 * Description: 
 * -----
 * Last Modified: 2022/7/2 09:33:07
 * Modified By: yang (yangyangyang0110@outlook.com)
 * -----
 * Copyright (c) 2022  . All rights reserved.
 */

#include "./utils.hpp"


template<unsigned v>
struct factorial {
  static constexpr unsigned long long value = v * factorial<v - 1>::value;
};

template<>
struct factorial<1> {
  static constexpr unsigned long long value = 1;
};

int main() {
  static_assert(is_one_of_v<int, int>);
  static_assert(not is_one_of_v<int, float>);
  static_assert(is_one_of_v<int, int, float, double>);
  static_assert(is_one_of_v<int, float, double, int>);
  static_assert(not is_one_of_v<int, float, double, unsigned long long>);
  static_assert(is_one_of_v<int, float, double, int, unsigned long long>);


  static_assert((6 * 5 * 4 * 3 * 2 * 1) == factorial<6>::value);
  static_assert((3 * 2 * 1) == factorial<3>::value);
  return 0;
}
