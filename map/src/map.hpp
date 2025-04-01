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

template<
    class Key,
    class T,
    class Compare = std::less <Key>
> class map {
 private:
  struct Node {
    Node *parent, *left, *right;
    bool is_black;
    Key *key;
    T *value;
    Node(Node *pa, Node *l = nullptr, Node *r = nullptr, bool isblack = false) :
        parent(pa), left(l), right(r), is_black(isblack), key(nullptr), value(nullptr) {}
    Node() = delete;
    Node(const Node &other) = delete;
    ~Node() {
      delete key;
      delete value;
    }
    Node& operator = (const Node &other) = delete;
    void rotate_right() {
      if (!left) throw sjtu::runtime_error("right rotation failed");
      Node *tmp_left = left;
      set_left(this, tmp_left->right);
      set_right(tmp_left, this);
      if (parent) {
        if (parent->left == *this) {
          parent->set_left(tmp_left);
        } else if (parent->right == *this) {
          parent->set_right(tmp_left);
        } else {
          throw sjtu::runtime_error("father-son relationship broken");
        }
      }
    }
    void rotate_left() {
      if (!right) throw sjtu::runtime_error("left rotation failed");
      Node *tmp_right = right;
      set_right(this, tmp_right->left);
      set_left(tmp_right, this);
      if (parent) {
        if (parent->left == *this) {
          parent->set_left(tmp_right);
        } else if (parent->right == *this) {
          parent->set_right(tmp_right);
        } else {
          throw sjtu::runtime_error("father-son relationship broken");
        }
      }
    }
  };
  inline bool is_black(const Node *ptr) {
    return !ptr || ptr->is_black;
  }
  inline void set_left(Node *parent, Node *son) {
    if (!parent) throw sjtu::runtime_error("set_left failed");
    parent->left = son;
    if (son) son->parent = parent;
  }
  inline void set_right(Node *parent, Node *son) {
    if (!parent) throw sjtu::runtime_error("set_right failed");
    parent->right = son;
    if (son) son->parent = parent;
  }
  Node *next(Node *ptr) {
    if (!ptr) throw sjtu::runtime_error("empty ptr for next");
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
  Node *prev(Node *ptr) {
    if (!ptr) throw sjtu::runtime_error("empty ptr for prev");
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
  Node *lower_bound(Node *ptr, const Key *key) {
    if (!ptr || !key) throw sjtu::runtime_error("empty ptr for lower_bound");
    while (true) {
      if (cmp(ptr->key, key)) {
        if (ptr->right) {
          ptr = ptr->right;
        } else {
          return ptr->right = new Node(ptr);
        }
      } else if (cmp(key, ptr->key)) {
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
  Node *root;
  size_t size_;
  Compare cmp;
 public:
  /**
   * the internal type of data.
   * it should have a default constructor, a copy constructor.
   * You can use sjtu::map as value_type by typedef.
   */
  typedef pair<const Key, T> value_type;
  /**
   * see BidirectionalIterator at CppReference for help.
   *
   * if there is anything wrong throw invalid_iterator.
   *     like it = map.begin(); --it;
   *       or it = map.end(); ++end();
   */
  class const_iterator;
  class iterator {
   private:
    /**
     * TODO add data members
     *   just add whatever you want.
     */
   public:
    iterator() {
      // TODO
    }

    iterator(const iterator &other) {
      // TODO
    }

    /**
     * TODO iter++
     */
    iterator operator++(int) {}

    /**
     * TODO ++iter
     */
    iterator &operator++() {}

    /**
     * TODO iter--
     */
    iterator operator--(int) {}

    /**
     * TODO --iter
     */
    iterator &operator--() {}

    /**
     * a operator to check whether two iterators are same (pointing to the same memory).
     */
    value_type &operator*() const {}

    bool operator==(const iterator &rhs) const {}

    bool operator==(const const_iterator &rhs) const {}

    /**
     * some other operator for iterator.
     */
    bool operator!=(const iterator &rhs) const {}

    bool operator!=(const const_iterator &rhs) const {}

    /**
     * for the support of it->first.
     * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
     */
    value_type *operator->() const
    noexcept {}
  };
  class const_iterator {
    // it should has similar member method as iterator.
    //  and it should be able to construct from an iterator.
   private:
    // data members.
   public:
    const_iterator() {
      // TODO
    }

    const_iterator(const const_iterator &other) {
      // TODO
    }

    const_iterator(const iterator &other) {
      // TODO
    }
    // And other methods in iterator.
    // And other methods in iterator.
    // And other methods in iterator.
  };

  /**
   * TODO two constructors
   */
  map() {}

  map(const map &other) {}

  /**
   * TODO assignment operator
   */
  map &operator=(const map &other) {}

  /**
   * TODO Destructors
   */
  ~map() {}

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

  /**
   * return a iterator to the beginning
   */
  iterator begin() {}

  const_iterator cbegin() const {}

  /**
   * return a iterator to the end
   * in fact, it returns past-the-end.
   */
  iterator end() {}

  const_iterator cend() const {}

  /**
   * checks whether the container is empty
   * return true if empty, otherwise false.
   */
  bool empty() const {}

  /**
   * returns the number of elements.
   */
  size_t size() const {}

  /**
   * clears the contents
   */
  void clear() {}

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
