/* Proj: experiments
 * File: demo.cpp
 * Created Date: 2022/7/6
 * Author: yangzilong (yangyangyang0110@outlook.com)
 * Description: 
 * -----
 * Last Modified: 2022/7/6 13:22:29
 * Modified By: yang (yangyangyang0110@outlook.com)
 * -----
 * Copyright (c) 2022  . All rights reserved.
 */
#include <cassert>
#include <cmath>
#include <functional>
#include <future>
#include <iostream>
#include <thread>
#include "./thread_pool.hpp"


template<typename Func, typename... Args, typename RetType = std::result_of_t<Func && (Args && ...)>>
// std::future<RetType>
std::packaged_task<RetType()> func_wrapper(Func &&func, Args &&...args) {
  // std::packaged_task<RetType(Args...)> task(std::forward<Func>(func));
  std::packaged_task<RetType()> task(std::bind(std::forward<Func>(func), std::forward<Args>(args)...));
  return task;
}


int fib(int n) { return (n < 3) ? n : fib(n - 1) + fib(n - 2); }


void test() {
  auto task = func_wrapper(fib, 10);
  auto f = task.get_future();
  std::string x = "this is a string.";
  auto task_wrap = [x = std::move(x), &task]() {
    printf("%s.\n", x.c_str());
    task();
  };
  task_wrap();
  assert(fib(10) == f.get());
}


// unique function to avoid disambiguating the std::pow overload set
int f(int x, int y) { return std::pow(x, y); }

void task_thread() {
  std::packaged_task<int(int, int)> task(f);
  std::future<int> result = task.get_future();

  std::thread task_td(std::move(task), 2, 10);
  task_td.join();

  std::cout << "task_thread:\t" << result.get() << '\n';
}

int main() {
  test();
  return 0;
}
