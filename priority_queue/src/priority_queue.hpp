#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {

template<class T>
class SharedPtr {
 private:
  T *get;
  size_t *cnt;
 public:
  SharedPtr() : get(nullptr), cnt(nullptr) {}
  SharedPtr(T *ptr) : get(ptr), cnt(new size_t{1}) {}
  SharedPtr(const SharedPtr &other) : get(other.get), cnt(other.cnt) {
    if (cnt) ++*cnt;
  }
  SharedPtr &operator=(const SharedPtr &other) {
    if (&other == this) return *this;
    this->reset();
    get = other.get;
    cnt = other.cnt;
    if (cnt != nullptr) ++*cnt;
    return *this;
  }
  void reset(T *ptr) {
    *this = SharedPtr(ptr);
  }
  void reset() {
    if (cnt && --*cnt == 0) {
      delete cnt;
      delete get;
    }
    cnt = nullptr;
    get = nullptr;
  }
	SharedPtr deep_copy() {
		if (get != nullptr) return SharedPtr(new T(*get));
		else return {};
	}
  const T& operator*() const { return *get; }
  T& operator*() { return *get; }
  const T* operator->() const { return get; }
  T* operator->() { return get; }
  ~SharedPtr() { reset(); }
};

template<class T>
class UniqueArray {
 private:
  T *arr;
 public:
  UniqueArray(size_t size) : arr(new T[size]) {}
  ~UniqueArray() { delete[] arr; }
  T& operator[](size_t x) {
    return arr[x];
  }
  const T& operator[](size_t x) const {
    return arr[x];
  }
};

/**
 * @brief a container like std::priority_queue which is a heap internal.
 * **Exception Safety**: The `Compare` operation might throw exceptions for certain data.
 * In such cases, any ongoing operation should be terminated, and the priority queue should be restored to its original state before the operation began.
 */
template<typename T, class Compare = std::less<T>>
class priority_queue { // pairing heap
 private:
  class Node {
    SharedPtr<const T> value;
    SharedPtr<Node> first_child;
    SharedPtr<Node> sibling;
    Node(const T & val, SharedPtr<Node> fc = SharedPtr<Node>(), SharedPtr<Node> sib = SharedPtr<Node>()) : 
        value(new T(val)), first_child(fc), sibling(sib) {}
    Node(const Node &other) : value(other.value.deep_copy()),
		    first_child(other.first_child.deep_copy()), sibling(other.sibling.deep_copy()) {}
  };
  SharedPtr<Node> root;
	size_t size_;
  std::function<bool(const T&, const T&)> cmp;
 public:
  /**
   * @brief default constructor
   */
  priority_queue() : root{}, size_{0}, cmp(Compare()) {}

  /**
   * @brief copy constructor
   * @param other the priority_queue to be copied
   */
  priority_queue(const priority_queue &other) : 
      root(other.root.deep_copy()), size_(other.size_), cmp(other.cmp) {}

  /**
   * @brief deconstructor
   */
  ~priority_queue() {}

  /**
   * @brief Assignment operator
   * @param other the priority_queue to be assigned from
   * @return a reference to this priority_queue after assignment
   */
  priority_queue &operator=(const priority_queue &other) {
		root = other.root.deep_copy();
		size_ = other.size_;
    cmp = other.cmp;
	}

  /**
   * @brief get the top element of the priority queue.
   * @return a reference of the top element.
   * @throws container_is_empty if empty() returns true
   */
  const T & top() const {
		return *root->value;
	}

  /**
   * @brief push new element to the priority queue.
   * @param e the element to be pushed
   */
  void push(const T &e) {
    SharedPtr<Node> new_heap = new Node(e);
    if (cmp(e, top())) {

    }
  }

  /**
   * @brief delete the top element from the priority queue.
   * @throws container_is_empty if empty() returns true
   */
  void pop() {}

  /**
   * @brief return the number of elements in the priority queue.
   * @return the number of elements.
   */
  size_t size() const { return size_; }

  /**
   * @brief check if the container is empty.
   * @return true if it is empty, false otherwise.
   */
  bool empty() const { return size_ == 0; }

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