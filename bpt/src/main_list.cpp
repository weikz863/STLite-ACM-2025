#include "tree.hpp"
#include <algorithm>
#include <iostream>
#include <cstring>
#include <climits>
using std::cin, std::cout, std::endl;
struct KeyAndValue {
  static size_t const N = 65;
  char str[N];
  int value;
  KeyAndValue(const char *str_ = "", int value_ = 0) : value(value_) {
    std::strncpy(str, str_, N);
  }
  bool operator < (const KeyAndValue &other) const {
    int i = std::strcmp(str, other.str);
    return i ? i < 0 : value < other.value;
  }
} ind;
BlockList<KeyAndValue, 110, FileStorage> list("only");
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
      auto result = list.find(KeyAndValue(ind.str, 0), KeyAndValue(ind.str, INT_MAX));
      if (result.size() == 0) {
        cout << "null\n";
      } else {
        for (const auto &tmp : result) {
          cout << tmp.value << ' ';
        }
        cout << '\n';
      }
    } else if (option[0] == 'd') { // "delete"
      cin >> ind.str >> ind.value;
      list.erase(ind);
    } else { // "insert"
      cin >> ind.str >> ind.value;
      list.insert(ind);
    }
  }
  return 0;
}