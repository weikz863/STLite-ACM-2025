/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

class my_runtime_error : public sjtu::exception {
 public:
  my_runtime_error(const char *s) {
    detail = std::string(s);
  }
};

template<
    class Key,
    class T,
    class Compare = std::less <Key>
> class map {
 public:
  /**
   * the internal type of data.
   * it should have a default constructor, a copy constructor.
   * You can use sjtu::map as value_type by typedef.
   */
  typedef pair<const Key, T> value_type;
 private:
  struct Node {
    Node *parent, *left, *right;
    bool is_black;
    value_type *info;
    Node(Node *pa, Node *l = nullptr, Node *r = nullptr, bool isblack = false) :
        parent(pa), left(l), right(r), is_black(isblack), info(nullptr) {}
    Node() = delete;
    Node(const Node &other) = delete;
    ~Node() {
      delete info;
      delete left;
      delete right;
    }
    Node& operator = (const Node &other) = delete;
    void rotate_right() {
      if (!left) throw sjtu::my_runtime_error("right rotation failed");
      Node *tmp_left = left;
      set_left(this, tmp_left->right);
      set_right(tmp_left, this);
      if (parent) {
        if (parent->left == *this) {
          parent->set_left(tmp_left);
        } else if (parent->right == *this) {
          parent->set_right(tmp_left);
        } else {
          throw sjtu::my_runtime_error("father-son relationship broken");
        }
      }
    }
    void rotate_left() {
      if (!right) throw sjtu::my_runtime_error("left rotation failed");
      Node *tmp_right = right;
      set_right(this, tmp_right->left);
      set_left(tmp_right, this);
      if (parent) {
        if (parent->left == *this) {
          parent->set_left(tmp_right);
        } else if (parent->right == *this) {
          parent->set_right(tmp_right);
        } else {
          throw sjtu::my_runtime_error("father-son relationship broken");
        }
      }
    }
  };
  static inline bool is_black(const Node *ptr) {
    return !ptr || ptr->is_black;
  }
  static inline void set_left(Node *parent, Node *son) {
    if (!parent) throw sjtu::my_runtime_error("set_left failed");
    parent->left = son;
    if (son) son->parent = parent;
  }
  static inline void set_right(Node *parent, Node *son) {
    if (!parent) throw sjtu::my_runtime_error("set_right failed");
    parent->right = son;
    if (son) son->parent = parent;
  }
  static Node *deep_copy(Node *ptr, Node *pa) {
    if (!pa) throw sjtu::my_runtime_error("deep copy with no parent");
    if (!ptr) return nullptr;
    Node *ret = new Node(pa);
    ret->left = deep_copy(ptr->left, ret);
    ret->right = deep_copy(ptr->right, ret);
    ret->is_black = ptr->is_black;
    if (!ptr->info) throw sjtu::my_runtime_error("deep copying virtual node");
    ret->info = new value_type(*ptr->info);
    return ptr;
  }
  static Node *next(Node *ptr) {
    if (!ptr) throw sjtu::my_runtime_error("empty ptr for next");
    if (ptr->right) {
      ptr = ptr->right;
      while (ptr->left) {
        ptr = ptr->left;
      }
    } else {
      while (ptr != ptr->parent->left) {
        ptr = ptr->parent;
      }
      ptr = ptr->parent;
    }
    return ptr;
  }
  static Node *prev(Node *ptr) {
    if (!ptr) throw sjtu::my_runtime_error("empty ptr for prev");
    if (ptr->left) {
      ptr = ptr->left;
      while (ptr->right) {
        ptr = ptr->right;
      }
    } else {
      while (ptr != ptr->parent->right) {
        ptr = ptr->parent;
      }
      ptr = ptr->parent;
    }
  }
  Node *try_find(Node *ptr, const Key &key) {
    if (!ptr) throw sjtu::my_runtime_error("empty ptr for try_find");
    while (true) {
      if (cmp(ptr->info.first, key)) {
        if (ptr->right) {
          ptr = ptr->right;
        } else {
          return ptr->right = new Node(ptr);
        }
      } else if (cmp(key, ptr->info.first)) {
        if (ptr->left) {
          ptr = ptr->left;
        } else {
          return ptr->left = new Node(ptr);
        }
      } else {
        return ptr;
      }
    }
  }
  Node *root, *begin_;
  size_t size_;
  Compare cmp;
 public:
  /**
   * see BidirectionalIterator at CppReference for help.
   *
   * if there is anything wrong throw invalid_iterator.
   *     like it = map.begin(); --it;
   *       or it = map.end(); ++end();
   */
  class const_iterator;
  class iterator {
    friend class const_iterator;
    friend class map;
   private:
    Node *ptr;
    iterator(Node *tmpptr) : ptr(tmpptr) {}
   public:
    iterator() : ptr(nullptr) {}
    iterator(const iterator &other) : ptr(other.ptr) {}

    iterator operator++(int) {
      if (ptr->parent == ptr) throw sjtu::invalid_iterator();
      Node *ret = ptr;
      ptr = next(ptr);
      return ret;
    }
    iterator &operator++() {
      if (ptr->parent == ptr) throw sjtu::invalid_iterator();
      ptr = next(ptr);
      return *this;
    }
    iterator operator--(int) {
      Node *ret = ptr;
      ptr = prev(ptr);
      if (ptr->parent == ptr) {
        ptr = ret;
        throw sjtu::invalid_iterator();
      }
      return ret;
    }
    iterator &operator--() {
      Node *tmp = ptr;
      ptr = prev(ptr);
      if (ptr->parent == ptr) {
        ptr = tmp;
        throw sjtu::invalid_iterator();
      }
      return *this;
    }

    value_type &operator*() const {
      return *ptr->info;
    }
    value_type *operator->() const noexcept {
      return ptr->info;
    }
    bool operator==(const iterator &rhs) const { return ptr == rhs.ptr; }
    bool operator==(const const_iterator &rhs) const { return ptr == rhs.ptr; }
    bool operator!=(const iterator &rhs) const { return ptr != rhs.ptr; }
    bool operator!=(const const_iterator &rhs) const { return ptr != rhs.ptr; }
  };
  class const_iterator {
    friend class iterator;
    friend class map;
   private:
    Node *ptr;
    const_iterator(Node *tmpptr) : ptr(tmpptr) {}
   public:
    const_iterator() : ptr(nullptr) {}
    const_iterator(const const_iterator &other) : ptr(other.ptr) {}
    const_iterator(const iterator &other) : ptr(other.ptr) {}
    
    const_iterator operator++(int) {
      if (ptr->parent == ptr) throw sjtu::invalid_iterator();
      Node *ret = ptr;
      ptr = next(ptr);
      return ret;
    }
    const_iterator &operator++() {
      if (ptr->parent == ptr) throw sjtu::invalid_iterator();
      ptr = next(ptr);
      return *this;
    }
    const_iterator operator--(int) {
      Node *ret = ptr;
      ptr = prev(ptr);
      if (ptr->parent == ptr) {
        ptr = ret;
        throw sjtu::invalid_iterator();
      }
      return ret;
    }
    const_iterator &operator--() {
      Node *tmp = ptr;
      ptr = prev(ptr);
      if (ptr->parent == ptr) {
        ptr = tmp;
        throw sjtu::invalid_iterator();
      }
      return *this;
    }

    const value_type &operator*() const {
      return *ptr->info;
    }
    const value_type *operator->() const noexcept {
      return ptr->info;
    }
    bool operator==(const iterator &rhs) const { return ptr == rhs.ptr; }
    bool operator==(const const_iterator &rhs) const { return ptr == rhs.ptr; }
    bool operator!=(const iterator &rhs) const { return ptr != rhs.ptr; }
    bool operator!=(const const_iterator &rhs) const { return ptr != rhs.ptr; }
  };

  map() : size_(0), cmp{} {
    root = static_cast<Node*>(::operator new(sizeof(Node)));
    ::new(root) Node(root, nullptr, root);
  }

  map(const map &other) : size_(other.size_), cmp{} {
    root = static_cast<Node*>(::operator new(sizeof(Node)));
    ::new(root) Node(root, deep_copy(other.root->left, root), root);
  }

  map &operator=(const map &other) {
    size_ = other.size_;
    delete root->left;
    root->left = deep_copy(other.root->left, root);
    return *this;
  }

  ~map() {
    root->right = nullptr;
    root->~Node();
    ::operator delete(root);
  }

  /**
   * TODO
   * access specified element with bounds checking
   * Returns a reference to the mapped value of the element with key equivalent to key.
   * If no such element exists, an exception of type `index_out_of_bound'
   */
  T &at(const Key &key) {}

  const T &at(const Key &key) const {}

  /**
   * TODO
   * access specified element
   * Returns a reference to the value that is mapped to a key equivalent to key,
   *   performing an insertion if such key does not already exist.
   */
  T &operator[](const Key &key) {
  }

  /**
   * behave like at() throw index_out_of_bound if such key does not exist.
   */
  const T &operator[](const Key &key) const {}

  iterator begin() { return next(root); }
  const_iterator cbegin() const { return next(root); }
  iterator end() { return root; }
  const_iterator cend() const { return root; }

  bool empty() const { return size_ == 0; }
  size_t size() const { return size_; }
  void clear() {
    delete root->left;
    root->left = nullptr;
    size_ = 0;
    begin_ = nullptr;
  }

  /**
   * insert an element.
   * return a pair, the first of the pair is
   *   the iterator to the new element (or the element that prevented the insertion),
   *   the second one is true if insert successfully, or false.
   */
  pair<iterator, bool> insert(const value_type &value) {}

  /**
   * erase the element at pos.
   *
   * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
   */
  void erase(iterator pos) {}

  /**
   * Returns the number of elements with key
   *   that compares equivalent to the specified argument,
   *   which is either 1 or 0
   *     since this container does not allow duplicates.
   * The default method of check the equivalence is !(a < b || b > a)
   */
  size_t count(const Key &key) const {}

  /**
   * Finds an element with key equivalent to key.
   * key value of the element to search for.
   * Iterator to an element with key equivalent to key.
   *   If no such element is found, past-the-end (see end()) iterator is returned.
   */
  iterator find(const Key &key) {}

  const_iterator find(const Key &key) const {}
};

}

#endif
