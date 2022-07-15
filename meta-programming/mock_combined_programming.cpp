/* Proj: experiments
 * File: mock_combined_programming.cpp
 * Created Date: 2022/7/3
 * Author: yangzilong (yangyangyang0110@outlook.com)
 * Description: 
 * -----
 * Last Modified: 2022/7/3 19:40:29
 * Modified By: yang (yangyangyang0110@outlook.com)
 * -----
 * Copyright (c) 2022  . All rights reserved.
 */

// template<typename T>
// concept StringAble;



#include <iostream>
#include <thread>


int main() {

  std::cout << std::thread::hardware_concurrency() << std::endl;

  return 0;
}