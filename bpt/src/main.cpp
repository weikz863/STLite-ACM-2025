#include "tree.hpp"
#include <algorithm>
#include <iostream>
#include <cstring>
using std::cin, std::cout, std::endl;
struct Index {
  char str[65];
  bool operator < (const Index &other) const {
    return std::strcmp(str, other.str) < 0;
  }
} ind;
BPlusTree<Index, int, 50> tmp;
int main() {
  std::ios::sync_with_stdio(false);
  cin.tie(nullptr);
  int n;
  char option[10];
  cin >> n;
  for (int i = 0; i < n; i++) {
    cin >> option;
    if (option[0] == 'f') { // "find"
      cin >> ind.str;
      auto result = tmp.find(ind);
      if (result.size() == 0) {
        cout << "null\n";
      } else {
        std::sort(result.begin(), result.end());
        for (const auto &tmp : result) {
          cout << tmp << ' ';
        }
        cout << '\n';
      }
    }
  }
  return 0;
}