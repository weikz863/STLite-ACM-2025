#ifndef SJTU_UTILITY_HPP
#define SJTU_UTILITY_HPP

#include <type_traits>
#include <functional>

namespace sjtu {

template <class T1, class T2>
class pair {
 public:
  using FirstType = T1;
  using SecondType = T2;
  T1 first;
  T2 second;
  constexpr pair() : first(), second() {}
  pair(const pair &other) = default;
  pair(pair &&other) = default;
  pair(const T1 &x, const T2 &y) : first(x), second(y) {}
  template <class U1, class U2>
  pair(U1 &&x, U2 &&y) : first(std::forward(x)), second(std::forward(y)) {}
  template <class U1, class U2>
  pair(const pair<U1, U2> &other) : first(other.first), second(other.second) {}
  template <class U1, class U2>
  pair(pair<U1, U2> &&other)
    : first(std::move(other.first)), second(std::move(other.second)) {}
};

}  // namespace sjtu

template <typename T>
struct is_sjtu_pair_with_int : std::false_type {};

template <typename U>
struct is_sjtu_pair_with_int<sjtu::pair<U, int>> : std::true_type {};

template <typename T>
struct AccumulativeFunc : public std::function<void(T)> {
  AccumulativeFunc() = default;
  template <typename... Args>
  AccumulativeFunc(Args... args) : std::function<void(T)>(std::forward<Args...>(args...)) {}
  AccumulativeFunc operator + (const AccumulativeFunc &other) {
    return [&] (T x) { this->operator()(x); other.operator()(x); };
  }
};

#endif