#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {

template<class T>
class shared_ptr {
 private:
  T *get;
  size_t *cnt;
 public:
  shared_ptr() : get(nullptr), cnt(nullptr) {}
  shared_ptr(T *ptr) : get(ptr), cnt(new size_t{1}) {}
  shared_ptr(const shared_ptr &other) : get(other.get), cnt(other.cnt) {
    if (cnt) ++*cnt;
  }
  shared_ptr &operator=(const shared_ptr &other) {
    if (&other == this) return *this;
    this->reset();
    get = other.get;
    cnt = other.cnt;
    if (cnt != nullptr) ++*cnt;
    return *this;
  }
  void reset(T *ptr) {
    *this = shared_ptr(ptr);
  }
  void reset() {
    if (cnt && --*cnt == 0) {
      delete cnt;
      delete get;
    }
    cnt = nullptr;
    get = nullptr;
  }
  const T &operator*() const { return *get; }
  T &operator*() { return *get; }
  ~shared_ptr() { reset(); }
};

/**
 * @brief a container like std::priority_queue which is a heap internal.
 * **Exception Safety**: The `Compare` operation might throw exceptions for certain data.
 * In such cases, any ongoing operation should be terminated, and the priority queue should be restored to its original state before the operation began.
 */
template<typename T, class Compare = std::less<T>>
class priority_queue { // pairing heap
 private:
  class node {
    shared_ptr<const T> value;
    shared_ptr<node> first_child;
    shared_ptr<node> sibling;
    node(const T & val, shared_ptr<node> fc = shared_ptr<node>(), shared_ptr<node> sib = shared_ptr<node>()) : 
        value(new T(val)), first_child(fc), sibling(sib) {}
    node(const node &other) : 
		    value(other.value), first_child(other.first_child), sibling(other.sibling) {}
  };
  shared_ptr<node> root;
 public:
  /**
   * @brief default constructor
   */
  priority_queue() : root{} {}

  /**
   * @brief copy constructor
   * @param other the priority_queue to be copied
   */
  priority_queue(const priority_queue &other) : root(new node(*other.root)) {}

  /**
   * @brief deconstructor
   */
  ~priority_queue() {}

  /**
   * @brief Assignment operator
   * @param other the priority_queue to be assigned from
   * @return a reference to this priority_queue after assignment
   */
  priority_queue &operator=(const priority_queue &other) {}

  /**
   * @brief get the top element of the priority queue.
   * @return a reference of the top element.
   * @throws container_is_empty if empty() returns true
   */
  const T & top() const {}

  /**
   * @brief push new element to the priority queue.
   * @param e the element to be pushed
   */
  void push(const T &e) {}

  /**
   * @brief delete the top element from the priority queue.
   * @throws container_is_empty if empty() returns true
   */
  void pop() {}

  /**
   * @brief return the number of elements in the priority queue.
   * @return the number of elements.
   */
  size_t size() const {}

  /**
   * @brief check if the container is empty.
   * @return true if it is empty, false otherwise.
   */
  bool empty() const {}

  /**
   * @brief merge another priority_queue into this one.
   * The other priority_queue will be cleared after merging.
   * The complexity is at most O(logn).
   * @param other the priority_queue to be merged.
   */
  void merge(priority_queue &other) {}
};

}

#endif