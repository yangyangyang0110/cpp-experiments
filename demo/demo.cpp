/* Proj: experiments
 * File: demo.cpp
 * Created Date: 2022/6/27
 * Author: yangzilong (yangyangyang0110@outlook.com)
 * Description: 
 * -----
 * Last Modified: 2022/6/27 00:47:42
 * Modified By: yang (yangyangyang0110@outlook.com)
 * -----
 * Copyright (c) 2022  . All rights reserved.
 */

#include <future>
#include <string>

namespace details {

} // namespace details


inline void f(std::string &&s) {
  printf("rvalue param.\n");
}

inline void f(std::string const &s) {
  printf("lvalue param.\n");
}

template<typename... Args>
void f_forward(Args &&...args) {
  f(std::forward<Args>(args)...);
}

inline std::string get_string_inline() {
  return "inline normal string";
}


class UnDefaultInitializeFoo {
  int x;
  float y;
  unsigned u;
  char z;
  void *ptr;

public:
  void CheckPrint() const noexcept {
    printf("x: %d, y: %f u: %u, z: %c, ptr(is_null): %d.\n", x, y, u, z, ptr == nullptr);
  }
};

void test() {
  std::string str = "this is a string object.";
  f_forward(get_string_inline());
  f_forward(str);

  UnDefaultInitializeFoo().CheckPrint();
}


int main() {
  test();
  return 0;
}
