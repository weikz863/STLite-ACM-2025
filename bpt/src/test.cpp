#include <utility>
namespace sjtu {
template <class T1, class T2>
class pair final {
 public:
  T1 first;
  T2 second;
  pair(const T1 &x, const T2 &y) : first(x), second(y) {}
  pair(const pair&) = default;
  pair(pair&&) = default;
  template <class U1, class U2>
  pair& operator = (const pair<U1, U2> &other) {
    first = other.first;
    second = other.second;
    return *this;
  }
  template <class U1, class U2>
  pair& operator = (pair<U1, U2> &&other) {
    first = std::move(other.first);
    second = std::move(other.second);
    return *this;
  }
};
}
using pii = sjtu::pair<int, int>;
int main() {
  pii pr1(0, 0), pr2(1, 2);
  const pii &x = pr2;
  pr1 = x;
}