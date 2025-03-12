#include <vector.hpp>

#include <iostream>

int main(void) {
  sjtu::vector<int> a;
  for (int i = 1; i <= 15; i++) a.push_back(i);
  sjtu::vector<int> b;
  b.push_back(1);
  b = std::move(a);
  std::cout << b[11] << std::endl;
  return 0;
}