/* Proj: experiments
 * File: dag.cpp
 * Created Date: 2022/6/27
 * Author: yangzilong (yangyangyang0110@outlook.com)
 * Description:
 * -----
 * Last Modified: 2022/6/27 00:47:42
 * Modified By: yang (yangyangyang0110@outlook.com)
 * -----
 * Copyright (c) 2022  . All rights reserved.
 */

/**
 * @todo: mock dag and it's data stream.
 */

#include <concepts>
#include <string>

/**
 *     ----- x2
 *   /
 * x1
 *   \
 *    ----- x3
 */

struct Graph {};

template<typename T = std::string>
struct Node {
  std::string name;
  T data;
};

struct NodeString : public Node<std::string> {};

template<typename T>
void printNode(T const &t)
  requires std::is_base_of_v<NodeString, T>
{
  printf("[Node]: {name=%s, data=\"%s\"}\n", t.name.c_str(), t.data.c_str());
}

void test() {
  NodeString x1{"x1", "x1-value"};
  NodeString x2{"x2", "x2-value"};
  NodeString x3{"x3", "x3-value"};

  printNode(x1);
  printNode(x2);
  printNode(x3);
}

int main() {
  test();
  return 0;
}
