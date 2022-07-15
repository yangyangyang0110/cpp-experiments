/* Proj: experiments
 * File: basic.cpp
 * Created Date: 2022/7/2
 * Author: yangzilong (yangyangyang0110@outlook.com)
 * Description: 
 * -----
 * Last Modified: 2022/7/2 12:21:53
 * Modified By: yang (yangyangyang0110@outlook.com)
 * -----
 * Copyright (c) 2022  . All rights reserved.
 */

#include "./utils.hpp"
#include <cstddef>

namespace details {

template<typename T>
struct remove_const : type_is<T> {};

template<typename T>
struct remove_const<T const> : type_is<T> {};

template<typename T>
struct remove_volatile : type_is<T> {};

template<typename T>
struct remove_volatile<T volatile> : type_is<T> {};

template<typename T>
struct remove_pointer : type_is<T> {};

template<typename T>
struct remove_pointer<T *> : type_is<T> {};

template<typename>
struct is_array : false_type {};

template<typename T, size_t N>
struct is_array<T[N]> : true_type {};

template<typename T>
struct is_array<T[]> : true_type {};

} // namespace details

template<typename T>
using remove_const_t = typename details::remove_const<T>::type;

template<typename T>
using remove_volatile_t = typename details::remove_volatile<T>::type;

/**
 * or
 * using remove_cv_t = typename details::remove_const<typename details::remove_volatile<T>::type>::type;
 */
template<typename T>
using remove_cv_t = remove_const_t<remove_volatile_t<T>>;


template<typename T>
using remove_pointer_t = typename details::remove_pointer<T>::type;


template<typename T>
inline static constexpr bool is_array_v = details::is_array<T>::value;

int main() {
  static_assert(is_same_v<remove_const_t<int const>, int>);
  static_assert(is_same_v<remove_volatile_t<int volatile>, int>);
  static_assert(not is_same_v<remove_const_t<int const *>, int>);
  static_assert(is_same_v<remove_const_t<int const *const>, int const *>);
  static_assert(is_same_v<remove_const_t<const int *>, int const *>);

  static_assert(is_same_v<remove_cv_t<int *const volatile>, int *>);
  static_assert(is_same_v<remove_cv_t<int const volatile *const volatile>, int const volatile *>);


  static_assert(is_same_v<remove_pointer_t<int *>, int>);
  static_assert(is_same_v<remove_pointer_t<int const *>, int const>);
  static_assert(is_same_v<remove_pointer_t<int *const>, int *const>); // top const.


  static_assert(is_array_v<int[123]>);
  static_assert(is_array_v<int[]>); // this also is array.


  return 0;
}
