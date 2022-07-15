/* Proj: experiments
 * File: utils.hpp
 * Created Date: 2022/7/2
 * Author: yangzilong (yangyangyang0110@outlook.com)
 * Description: 
 * -----
 * Last Modified: 2022/7/2 11:55:42
 * Modified By: yang (yangyangyang0110@outlook.com)
 * -----
 * Copyright (c) 2022  . All rights reserved.
 */
#ifndef EXPERIMENTS_UTILS_HPP
#define EXPERIMENTS_UTILS_HPP


#include <cstddef>
#include <type_traits>


namespace details {

template<typename T, T v>
struct integral_constant {
  using value_type = T;
  static constexpr value_type value = v;
  // using type = value_type;
  constexpr operator value_type() const noexcept { return value; }
  constexpr value_type operator()() const noexcept { return value; }
};


template<bool b>
using bool_constant = integral_constant<bool, b>;
using true_type = bool_constant<true>;
using false_type = bool_constant<false>;


template<typename T, typename U>
struct is_same : false_type {};

template<typename T>
struct is_same<T, T> : true_type {};


template<typename>
struct void_type {
  using type = void;
};

template<typename T>
using void_t = typename details::void_type<T>::type;

template<typename T, typename... Types>
struct is_one_of : false_type {};

template<typename T, typename... Types>
struct is_one_of<T, T, Types...> : true_type {};

template<typename T, typename U, typename... Types>
struct is_one_of<T, U, Types...> : is_one_of<T, Types...> {};


template<typename T>
struct type_is {
  using type = T;
};

template<bool, typename T, typename F>
struct IF : type_is<T> {};

template<typename T, typename F>
struct IF<false, T, F> : type_is<F> {};


template<bool b, typename Type = void>
struct enable_if {};

template<typename Type>
struct enable_if<true, Type> : type_is<Type> {};

template<typename, typename = void>
struct has_type_member : false_type {};

template<typename T>
struct has_type_member<T, void_t<typename T::type>> : true_type {};

template<typename>
struct rank {
  static constexpr int value = 0;
};

template<typename T, size_t N>
struct rank<T[N]> {
  static constexpr int value = 1 + rank<T>::value;
};

template<typename T>
struct rank<T[]> {
  static constexpr int value = 1 + rank<T>::value;
};

template<typename T>
using copy_assignment_t = decltype(std::declval<T &>() = std::declval<T const &>());

template<typename T>
using move_assignment_t = decltype(std::declval<T &>() = std::declval<T &&>());


template<typename T, typename = void>
struct has_copy_assignment : false_type {};

template<typename T>
struct has_copy_assignment<T, typename void_type<copy_assignment_t<T>>::type> : is_same<T &, copy_assignment_t<T>> {};


template<typename T, typename = void>
struct has_move_assignment : false_type {};

template<typename T>
struct has_move_assignment<T, typename void_type<move_assignment_t<T>>::type> : is_same<T &, move_assignment_t<T>> {};


} // namespace details


template<typename T, typename U>
inline static constexpr bool is_same_v = details::is_same<T, U>::value;

template<typename T, typename... Types>
inline static constexpr bool is_one_of_v = details::is_one_of<T, Types...>::value;

template<bool b, typename T, typename F>
using if_t = typename details::IF<b, T, F>::type;

template<bool b, typename Type = void>
using enable_if_t = typename details::enable_if<b, Type>::type;

template<typename T>
inline static constexpr bool has_type_member_v = details::has_type_member<T>::value;

template<typename Tp, size_t N>
inline static constexpr size_t arr_size(const Tp (&)[N]) noexcept {
  return N;
}

template<typename T>
inline static constexpr int rank_v = details::rank<T>::value;

template<typename T>
inline static constexpr bool has_copy_assignment_v = details::has_copy_assignment<T>::value;

template<typename T>
inline static constexpr bool has_move_assignment_v = details::has_move_assignment<T>::value;

using details::void_t;

#endif //EXPERIMENTS_UTILS_HPP
