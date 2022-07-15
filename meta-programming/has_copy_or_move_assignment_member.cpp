/* Proj: experiments
 * File: has_copy_or_move_assignment_member.cpp
 * Created Date: 2022/7/2
 * Author: yangzilong (yangyangyang0110@outlook.com)
 * Description: 
 * -----
 * Last Modified: 2022/7/2 22:40:12
 * Modified By: yang (yangyangyang0110@outlook.com)
 * -----
 * Copyright (c) 2022  . All rights reserved.
 */

#include "./utils.hpp"

struct S1 {
  S1 &operator=(S1 const &) = default;
  S1 &operator=(S1 &&) = delete;
};

struct S2 {
  S2 &operator=(S2 const &) = delete;
  S2 &operator=(S2 &&) = default;
};

void test() {
  static_assert(has_copy_assignment_v<S1>);
  static_assert(not has_move_assignment_v<S1>);
  static_assert(not has_copy_assignment_v<S2>);
  static_assert(has_move_assignment_v<S2>);
}

int main() {
  test();
  return 0;
}