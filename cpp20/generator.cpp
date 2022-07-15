/* Proj: experiments
 * File: generator.cpp
 * Created Date: 2022/7/3
 * Author: yangzilong (yangyangyang0110@outlook.com)
 * Description: 
 * -----
 * Last Modified: 2022/7/3 16:59:38
 * Modified By: yang (yangyangyang0110@outlook.com)
 * -----
 * Copyright (c) 2022  . All rights reserved.
 */


#include <cstdio>
/**
 * @brief 生成[1, N]生成器.
 * @return
 */

void c11_style() {
  auto g = [i = 0]() mutable { return ++i; };

  for (int i = 0; i < 10; ++i) {
    printf("value -> %d\n", g());
  }
}


void test() { c11_style(); }


int main() {
  test();
  return 0;
}
