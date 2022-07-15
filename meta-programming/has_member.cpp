/* Proj: experiments
 * File: has_member.cpp
 * Created Date: 2022/7/2
 * Author: yangzilong (yangyangyang0110@outlook.com)
 * Description: 
 * -----
 * Last Modified: 2022/7/2 10:24:37
 * Modified By: yang (yangyangyang0110@outlook.com)
 * -----
 * Copyright (c) 2022  . All rights reserved.
 */

#include "./utils.hpp"
#include <string>

/**
 * @brief mock scene:
 * @return
 */


/**
 * @brief 判断是否是class
 * 1. function override.
 * 2. SFINAE
 * @return
 */

namespace details {

/**
 * @brief P.S: std::is_union_v impl by difference compiler...
 */

/**
 * @brief implement using struct style.
 * @tparam T
 * @return
 */
template<typename T>
static constexpr bool_constant<!std::is_union_v<T>> is_class(int(T::*));

template<typename>
static constexpr false_type is_class(...);

/**
 * @brief implement using function style.
 * @tparam T
 * @return
 */
template<typename T>
static constexpr bool is_class_function_impl(int T::*) {
  return bool_constant<!std::is_union_v<T>>::value;
}

template<typename>
static constexpr bool is_class_function_impl(...) {
  return false;
}

} // namespace details

template<typename T>
using is_class_v = decltype(details::is_class<T>(nullptr));


template<typename T>
inline static constexpr bool is_class_function_impl_v = details::is_class_function_impl<T>(nullptr);

struct C1 {
public:
  C1() = delete;
  std::string to_string() { return "C1"; }
};

class C2 {
public:
  C2() = delete;
  std::string to_string() { return "C2"; }
};

union C3 {
  class C4 {};
};

enum class E {

};

int main() {
  static_assert(is_same_v<std::string, decltype(std::declval<C2>().to_string())>);

  static_assert(is_class_v<C1>());
  static_assert(is_class_v<C2>());
  static_assert(not is_class_v<int>());

  static_assert(not is_class_v<C3>());
  static_assert(is_class_v<C3::C4>());
  static_assert(not is_class_v<E>());

  static_assert(is_class_function_impl_v<C1>);
  static_assert(is_class_function_impl_v<C2>);
  static_assert(not is_class_function_impl_v<int>);

  static_assert(not is_class_function_impl_v<C3>);
  static_assert(is_class_function_impl_v<C3::C4>);
  static_assert(not is_class_function_impl_v<E>);

  return 0;
}
