#include "tree.hpp"
#include <algorithm>
#include <random>
#include <iostream>
#include <vector>
using std::cout;
int const N = 500, N1 = 100, N2 = 300, M = 2;
int arr[N];
int main() {
  for (int i = 0; i < N; i++) arr[i] = i;
  for (int j = 0; j < 10000; j++) {
    BlockBlockList<int, 5, VectorStorage> bbl("temp");
    std::random_shuffle(arr, arr + N);
    for (int i = 0; i < N2; i++) {
      bbl.insert(arr[i]);
    }
    for (int i = N1; i < N2; i++) {
      bbl.erase(arr[i]);
    }
    std::vector ans(arr, arr + N1);
    for (int i = N2; i < N; i++) {
      bbl.insert(arr[i]);
      ans.push_back(arr[i]);
    }
    int const min = M, max = N - M;
    for (int i = 0; i < ans.size(); i++) {
      if (ans[i] < min || ans[i] > max) {
        ans[i] = ans.back();
        ans.pop_back();
        i--;
      }
    }
    std::sort(ans.begin(), ans.end());
    auto t = bbl.find(min, max);
    if (t.size() != ans.size()) {
      cout << "FAILED test:" << j << "\noutput = ";
      for (auto x : t) {
        cout << x << ' ';
      }
      cout << "\nans = ";
      for (auto x : ans) {
        cout << x << ' ';
      }
      cout << '\n';
      return 0;
    }
    for (int i = 0; i < ans.size(); i++) {
      if (t[i] != ans[i]) {
        cout << "FAILED\n";
        return 0;
      }
    }
  }
  cout << "PASSED\n";
}