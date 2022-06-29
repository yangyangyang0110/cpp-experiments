/* Proj: experiments
 * File: main.cpp
 * Created Date: 2022/6/27
 * Author: yangzilong (yangyangyang0110@outlook.com)
 * Description: 
 * -----
 * Last Modified: 2022/6/27 00:47:42
 * Modified By: yang (yangyangyang0110@outlook.com)
 * -----
 * Copyright (c) 2022  . All rights reserved.
 */

#include <concepts>
#include <iostream>
#include <string>

template<typename T>
concept toStringAble = requires(T t) {
                         { t.to_string() };
                       };

template<toStringAble T>
void to_string(T &&t) {
  std::cout << "c20: " << t.to_string() << std::endl;
}


template<typename T>
auto to_string_c11(T &&t) -> typename std::enable_if<std::is_same<decltype(t.to_string()), std::string>::value>::type {
  std::cout << "c11: " << t.to_string() << std::endl;
}

struct True {
  std::string to_string() const { return "True"; }
};

struct False {};


void test() {
  True obj;
  to_string(obj);
  to_string_c11(obj);

  // False obj2;
  // to_string(obj2);
  // to_string_c11(obj2);
}

int main() {
  test();
  return 0;
}
