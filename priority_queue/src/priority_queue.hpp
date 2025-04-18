#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

#undef __VERBOSE__
#undef __MYTEST__

#ifdef __VERBOSE__
#include <iostream>
#endif

#ifdef __MYTEST__

#include <memory>

template <typename T>
class SharedPtr : public std::shared_ptr<T> {
 public:
  using std::shared_ptr<T>::shared_ptr;
  SharedPtr deep_copy() const {
    if (*this) return SharedPtr(new T(**this));
    else return {};
  }
};

#endif

namespace sjtu {

#ifndef __MYTEST__ 
template<class T>
class SharedPtr {
 private:
  T *get;
  size_t *cnt;
 public:
  SharedPtr(std::nullptr_t x = nullptr) : get(nullptr), cnt(nullptr) {}
  SharedPtr(T *ptr) : get(ptr), cnt(new size_t{1}) {}
  SharedPtr(const SharedPtr &other) : get(other.get), cnt(other.cnt) {
    if (cnt) ++*cnt;
  }
  SharedPtr(SharedPtr &&other) : get(other.get), cnt(other.cnt) {
    other.get = nullptr;
    other.cnt = nullptr;
  }
  SharedPtr &operator=(const SharedPtr &other) {
    if (&other == this) return *this;
    if (cnt && --*cnt == 0) {
      delete cnt;
      delete get;
    }
    get = other.get;
    cnt = other.cnt;
    if (cnt != nullptr) ++*cnt;
    return *this;
  }
  SharedPtr &operator=(SharedPtr &&other) {
    if (&other == this) return *this;
    if (cnt && --*cnt == 0) {
      delete cnt;
      delete get;
    }
    get = other.get;
    cnt = other.cnt;
    other.get = nullptr;
    other.cnt = nullptr;
    return *this;
  }
  void reset(T *ptr) {
    if (cnt && --*cnt == 0) {
      delete cnt;
      delete get;
    }
    get = ptr;
    cnt = new size_t{1};
  }
  void reset() {
    if (cnt && --*cnt == 0) {
      delete cnt;
      delete get;
    }
    cnt = nullptr;
    get = nullptr;
    #ifdef __VERBOSE__
    std::cout << "RE" << std::endl;
    #endif
  }
  SharedPtr deep_copy() const {
    if (get != nullptr) return SharedPtr(new T(*get));
    else return {};
  }
  void swap(SharedPtr &other) {
    if (&other == this) return;
    auto tget = get;
    auto tcnt = cnt;
    get = other.get;
    cnt = other.cnt;
    other.get = tget;
    other.cnt = tcnt;
  }
  explicit operator bool() const {
    return get != nullptr;
  }
  const T& operator*() const { return *get; }
  T& operator*() { return *get; }
  const T* operator->() const { return get; }
  T* operator->() { return get; }
  ~SharedPtr() {
    if (cnt && --*cnt == 0) {
      delete cnt;
      delete get;
    }
  }
};

#endif

/* NOT NEEDED WITH RECURSION 
template<class T>
class UniqueArray {
 private:
  T *arr;
 public:
  UniqueArray(size_t size = 0) : arr(new T[size]) {}
  ~UniqueArray() { delete[] arr; }
  UniqueArray(const UniqueArray &) = delete;
  T& operator[](size_t x) {
    return arr[x];
  }
  const T& operator[](size_t x) const {
    return arr[x];
  }
};
*/

/**
 * @brief a container like std::priority_queue which is a heap internal.
 * **Exception Safety**: The `Compare` operation might throw exceptions for certain data.
 * In such cases, any ongoing operation should be terminated, and the priority queue should be restored to its original state before the operation began.
 */
template<typename T, class Compare = std::less<T>>
class priority_queue { // pairing heap
 private:
  class Node {
   private:
    SharedPtr<const T> value;
    SharedPtr<Node> first_child;
    SharedPtr<Node> sibling;
   public:
    Node(const T & val, SharedPtr<Node> fc = SharedPtr<Node>(), SharedPtr<Node> sib = SharedPtr<Node>()) : 
        value(new T(val)), first_child(fc), sibling(sib) {}
    Node(SharedPtr<const T> val, SharedPtr<Node> fc = SharedPtr<Node>(), SharedPtr<Node> sib = SharedPtr<Node>()) : 
        value(val), first_child(fc), sibling(sib) {}
    Node(const Node &other) : value(other.value.deep_copy()),
        first_child(other.first_child.deep_copy()), sibling(other.sibling.deep_copy()) {
          #ifdef __VERBOSE__
          std::cout << "DEEP COPYING" << std::endl;
          #endif
        }
    Node& operator = (const Node &) = delete;
    priority_queue meld() const {
      #ifdef __VERBOSE__
      // std::cout << "MELDING, this = " << *this->value << std::endl;
      #endif
      priority_queue ret(*this);
      if (sibling) {
        ret.merge(priority_queue(*sibling));
        if (sibling->sibling) {
          ret.merge(sibling->sibling->meld());
        }
      }
      return ret;
    }
    friend class priority_queue;
  };
  SharedPtr<Node> root;
  size_t size_;
  std::function<bool(const T&, const T&)> cmp;
  priority_queue(const Node &rt) : root(new Node(rt.value, rt.first_child)),
      size_(1), cmp(Compare()) {} // WARNING: size not accurate, but need to be at least 1.
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
  priority_queue(priority_queue &&other) : 
      root{}, size_(other.size_), cmp(other.cmp) {
    root.swap(other.root);
  }
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
    if (&other == this) return *this;
    root = other.root.deep_copy();
    size_ = other.size_;
    cmp = other.cmp;
    return *this;
  }
  priority_queue &operator=(priority_queue &&other) {
    if (&other == this) return *this;
    root = std::move(other.root);
    size_ = other.size_;
    other.size_ = 0;
    return *this;
  }

  void swap(priority_queue &other) {
    if (&other == this) return;
    root.swap(other.root);
    auto tsize = size_;
    size_ = other.size_;
    other.size_ = tsize;
  }
  
  void clear() {
    root.reset();
    size_ = 0;
  }

  /**
   * @brief get the top element of the priority queue.
   * @return a reference of the top element.
   * @throws container_is_empty if empty() returns true
   */
  const T & top() const {
    if (size_ == 0) throw container_is_empty();
    return *root->value;
  }

  /**
   * @brief push new element to the priority queue.
   * @param e the element to be pushed
   */
  void push(const T &e) {
    if (size_ == 0) {
      size_ = 1;
      root.reset(new Node(e));
      return;
    }
    SharedPtr<Node> new_node(new Node(e));
    if (cmp(e, *root->value)) {
      new_node->sibling = root->first_child;
      root->first_child = new_node;
    } else {
      new_node->first_child = root;
      root = new_node;
    }
    size_++;
  }

  /**
   * @brief delete the top element from the priority queue.
   * @throws container_is_empty if empty() returns true
   */
  void pop() {
    if (size_ == 0) throw container_is_empty();
    if (!root->first_child) {
      clear();
      return;
    }
    size_t tsize = size_;
    root->first_child->meld().swap(*this);
    size_ = tsize - 1;
  }

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
  void merge(priority_queue &other) {
    if (&other == this) return;
    if (other.empty()) return;
    if (empty()) {
      swap(other);
      return;
    }
    if (cmp(*root->value, *other.root->value)) {
      root->sibling = other.root->first_child;
      other.root->first_child = root;
      swap(other);
    } else {
      other.root->sibling = root->first_child;
      root->first_child = other.root;
    }
    size_ += other.size_;
    other.clear();
  }
  void merge(priority_queue &&other) {
    if (&other == this) return;
    if (other.empty()) return;
    if (empty()) {
      swap(other);
      return;
    }
    if (cmp(*root->value, *other.root->value)) {
      root->sibling = other.root->first_child;
      other.root->first_child = root;
      swap(other);
    } else {
      other.root->sibling = root->first_child;
      root->first_child = other.root;
    }
    size_ += other.size_;
    other.clear();
  }
};

}

#endif