#ifndef SJTU_UTILITY_HPP
#define SJTU_UTILITY_HPP

#include <type_traits>
#include <functional>

namespace sjtu {

template <class T1, class T2>
class pair final {
 public:
  using FirstType = T1;
  using SecondType = T2;
  T1 first;
  T2 second;
  constexpr pair() : first(), second() {}
  pair(const pair &other) = default;
  pair(pair &&other) = default;
  template <class U1, class U2>
  pair(const U1 &x, const U2 &y) : first(x), second(y) {}
  template <class U1, class U2>
  pair(U1 &&x, U2 &&y) : first(std::forward<U1&&>(x)), second(std::forward<U2&&>(y)) {}
  template <class U1, class U2>
  pair(const pair<U1, U2> &other) : first(other.first), second(other.second) {}
  template <class U1, class U2>
  pair(pair<U1, U2> &&other)
    : first(std::move(other.first)), second(std::move(other.second)) {}
  pair& operator = (const pair &) = default;
  pair& operator = (pair &&) = default;
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
  bool operator < (const pair &other) const {
    return first < other.first || (!(other.first < first) && second < other.second);
  }
};

}  // namespace sjtu

template <typename T>
struct is_sjtu_pair_with_int : std::false_type {};

template <typename U>
struct is_sjtu_pair_with_int<sjtu::pair<U, int>> : std::true_type {};

template <typename T>
T& extract_data(T& x) { return x; }

template <typename T>
const T& extract_data(const T& x) { return x; }

template <typename U>
U& extract_data(sjtu::pair<U, int>& x) { return x.first; }

template <typename U>
const U& extract_data(const sjtu::pair<U, int>& x) { return x.first; }

template <typename T>
struct AccumulativeFunc : public std::function<void(T)> {
  AccumulativeFunc() = default;
  template <typename... Args>
  AccumulativeFunc(Args... args) : std::function<void(T)>(std::forward<Args...>(args...)) {}
  AccumulativeFunc operator + (const AccumulativeFunc &other) {
    return [&] (T x) { this->operator()(x); other.operator()(x); };
  }
};

template<typename T1, typename T2>
requires (std::is_trivially_copyable<T1>::value && std::is_trivially_copyable<T2>::value)
struct trivial_pair final {
  T1 first;
  T2 second;
  bool operator < (const trivial_pair& other) {
    return first < other.first || (!(other.first < first) && second < other.second);
  }
};

template<typename T1, typename T2>
requires (std::is_trivially_copyable<T1>::value && std::is_trivially_copyable<T2>::value)
trivial_pair<T1, T2> make_trivial_pair(const T1 &x, const T2 &y) {
  return {x, y};
};

#endif