/* Proj: experiments
 * File: template-special.cpp
 * Created Date: 2022/7/1
 * Author: yangzilong (yangyangyang0110@outlook.com)
 * Description: 
 * -----
 * Last Modified: 2022/7/1 23:51:58
 * Modified By: yang (yangyangyang0110@outlook.com)
 * -----
 * Copyright (c) 2022  . All rights reserved.
 */


#include <cassert>
#include <cstdio>
#include <type_traits>
/**
 * @brief struct special.
 * @return
 */

template<bool, typename>
struct Foo {
  Foo() { printf("initialize in S.\n"); }
};


template<>
struct Foo<false, int> {
  Foo() { printf("initialize in <false, int>.\n"); }
};


template<typename T>
struct Foo<false, T> {
  Foo() { printf("initialize in <false, T>.\n"); }
};


template<typename T>
struct Foo<true, T> {
  Foo() { printf("initialize in <true, T>.\n"); }
};

template<typename T>
bool pow(T t) {
  return t % 2 == 0;
}

template<>
bool pow<float>(float) {
  return false;
}


template<typename T>
  requires std::is_integral_v<T> bool
pow_v2(T t) {
  return t % 2 == 0;
}

template<typename T>
  requires std::is_floating_point_v<T> bool
pow_v2(T) {
  return false;
}


template<typename T, std::enable_if_t<std::is_integral_v<T>, void *> = nullptr>
bool pow_v3(T t) {
  return t % 2 == 0;
}

template<typename T, std::enable_if_t<std::is_floating_point_v<T>, void *> = nullptr>
bool pow_v3(T) {
  return false;
}


/**
 * @brief error:  c++ 函数重载默认不计算类型模板参数.
 * @tparam T
 */
/*
template<typename T, typename = std::enable_if_t<std::is_integral_v<T>, void *> = nullptr>
bool pow_v4(T t) {
  return t % 2 == 0;
}

template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>, void *> = nullptr>
bool pow_v4(T) {
  return false;
}
 */


int main() {

  Foo<true, unsigned> foo1;
  Foo<false, int> foo2;
  Foo<false, char> foo3;
  Foo<true, char> foo4;


  assert(pow(10));
  assert(not pow(20.3f));


  assert(pow_v2(10));
  assert(not pow_v2(20.3f));


  assert(pow_v3(10));
  assert(not pow_v3(20.3f));

  return 0;
}
