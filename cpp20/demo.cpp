/* Proj: experiments
 * File: demo.cpp
 * Created Date: 2022/7/2
 * Author: yangzilong (yangyangyang0110@outlook.com)
 * Description: 
 * -----
 * Last Modified: 2022/7/2 10:09:50
 * Modified By: yang (yangyangyang0110@outlook.com)
 * -----
 * Copyright (c) 2022  . All rights reserved.
 */

#include <cstdio>
#include <type_traits>

struct FooA {
  void a() const noexcept { printf("FooA::a.\n"); }
  void b() const noexcept { printf("FooA::b.\n"); }
};

struct FooB {
  void b() const noexcept { printf("FooB::b.\n"); }
};

struct FooC {
  void c() const noexcept { printf("FooC::c.\n"); }
};

struct FalseFoo {};

void f(auto &&t) {
  if constexpr (requires { t.a(); }) {
    t.a();
  } else if constexpr (requires { t.b(); }) {
    t.b();
  } else {
    t.c();
  }
}

void test() {
  f(FooA());
  f(FooB());
  f(FooC());
  // f(FalseFoo());

  char *ptr;
  printf("pointer size: %lu.\n", sizeof(ptr));
}

int main() {
  test();

  return 0;
}