#include "file.hpp"
#include <iostream>
using std::cin, std::cout, std::endl;
FileStorage tmp("name");
int main() {
  int a[] = {1,2,1325};
  tmp.write_at(0, a);
  tmp.write_at(3 * sizeof(int), 435);
  int *p = new int;
  tmp.read_at(sizeof(int), a);
  cout << tmp.initialized() << ' ' << a[0] << a[1] << a[2] << endl;
  delete p;
  return 0;
}