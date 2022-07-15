/* Proj: experiments
 * File: if_expression.cpp
 * Created Date: 2022/7/2
 * Author: yangzilong (yangyangyang0110@outlook.com)
 * Description: 
 * -----
 * Last Modified: 2022/7/2 00:22:05
 * Modified By: yang (yangyangyang0110@outlook.com)
 * -----
 * Copyright (c) 2022  . All rights reserved.
 */
#include "./utils.hpp"
#include <cstdio>

struct F {
  F() { printf("initialize F\n"); }
};

struct G {
  G() { printf("initialize G\n"); }
};


int main() {
  if_t<true, int, unsigned> v;
  if_t<true, F, G>{};
  if_t<false, F, G>{};
  return 0;
}
