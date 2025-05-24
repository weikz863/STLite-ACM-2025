#include "blockblocklist.hpp"
#include <iostream>
#include <cstring>
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
  bool operator == (const KeyAndValue &other) const = delete;
} ind;
BlockBlockList<KeyAndValue, 161, FileStorage> tree("list");
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
      auto result = tree.find(KeyAndValue(ind.str, INT_MIN), KeyAndValue(ind.str, INT_MAX));
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
      tree.erase(ind);
    } else { // "insert"
      cin >> ind.str >> ind.value;
      tree.insert(ind);
    }
  }
  return 0;
}