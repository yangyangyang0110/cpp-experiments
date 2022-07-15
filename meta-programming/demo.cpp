/* Proj: experiments
 * File: demo.cpp
 * Created Date: 2022/7/1
 * Author: yangzilong (yangyangyang0110@outlook.com)
 * Description: 
 * -----
 * Last Modified: 2022/7/1 21:55:52
 * Modified By: yang (yangyangyang0110@outlook.com)
 * -----
 * Copyright (c) 2022  . All rights reserved.
 */


#include "./utils.hpp"
#include <cstdio>
/**
 * @brief get array size.
 */


template<typename T, size_t N, size_t N2>
void get_array_size(const T (&)[N][N2]) {
  printf("[%lu][%lu].\n", N, N2);
}


void test() {
  int x[100][200];
  get_array_size(x);
}


int main() {
  test();
  return 0;
}