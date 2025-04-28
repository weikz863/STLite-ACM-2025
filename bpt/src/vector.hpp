#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>

/*
template< class T, class... Args >
T* std::construct_at( T* location, Args&&... args ) {
  return new (*location) T(std::forward<Args>(args)...);
}
template< class T >
void std::destroy_at( T* p ) {
  p->~T();
}
*/
namespace sjtu
{
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
template<typename T>
class vector
{
private:
  std::allocator<T> alloc;
  T* arr;
  size_t capacity_, size_;
  const static size_t kSMALL = 10;
  void try_shrink() {
    if (size_ < capacity_ / 4 && capacity_ / 2 >= kSMALL) {
      T* other = alloc.allocate(capacity_ / 2);
      for (size_t i = 0; i < size_; i++) {
        std::construct_at(other + i, std::move(arr[i]));
        std::destroy_at(arr + i);
      }
      alloc.deallocate(arr, capacity_);
      arr = other;
      capacity_ /= 2;
    }
  }
  void try_double() {
    if (size_ == capacity_) {
      T* other = alloc.allocate(capacity_ * 2);
      for (size_t i = 0; i < size_; i++) {
        std::construct_at(other + i, std::move(arr[i]));
        std::destroy_at(arr + i);
      }
      alloc.deallocate(arr, capacity_);
      arr = other;
      capacity_ *= 2;
    }
  }
public:
  /**
   * a type for actions of the elements of a vector, and you should write
   *   a class named const_iterator with same interfaces.
   */
  class const_iterator;
  class iterator {
		friend class vector;
  // The following code is written for the C++ type_traits library.
  // Type traits is a C++ feature for describing certain properties of a type.
  // For instance, for an iterator, iterator::value_type is the type that the
  // iterator points to.
  // STL algorithms and containers may use these type_traits (e.g. the following
  // typedef) to work properly. In particular, without the following code,
  // @code{std::sort(iter, iter1);} would not compile.
  // See these websites for more information:
  // https://en.cppreference.com/w/cpp/header/type_traits
  // About value_type: https://blog.csdn.net/u014299153/article/details/72419713
  // About iterator_category: https://en.cppreference.com/w/cpp/iterator
  public:
    using difference_type = int;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::output_iterator_tag;

  private:
		const T* init;
		T* ptr;
		iterator(const T* init_, T* ptr_) : init(init_), ptr(ptr_) {}
  public:
    /**
     * return a new iterator which pointer n-next elements
     * as well as operator-
     */
    iterator operator+(const int &n) const
    {
      return {init, ptr + n};
    }
    iterator operator-(const int &n) const
    {
      return {init, ptr - n};
    }
    // return the distance between two iterators,
    // if these two iterators point to different vectors, throw invaild_iterator.
    int operator-(const iterator &rhs) const
    {
       if (init != rhs.init) throw invalid_iterator();
			 return ptr - rhs.ptr;
    }
    iterator& operator+=(const int &n)
    {
      ptr += n;
			return *this;
    }
    iterator& operator-=(const int &n)
    {
      ptr -= n;
			return *this;
    }
    iterator operator++(int) {
			return {init, ptr++};
		}
    iterator& operator++() {
			ptr++;
			return *this;
		}
    iterator operator--(int) {
			return {init, ptr--};
		}
    iterator& operator--() {
			ptr--;
			return *this;
		}
    T& operator*() const { return *ptr; }
    bool operator==(const iterator &rhs) const { return ptr == rhs.ptr; }
    bool operator==(const const_iterator &rhs) const { return ptr == rhs.ptr; }
    bool operator!=(const iterator &rhs) const { return ptr != rhs.ptr; }
    bool operator!=(const const_iterator &rhs) const { return ptr != rhs.ptr; }
  };
  /**
   * has same function as iterator, just for a const object.
   */
  class const_iterator
  {
		friend class vector;
  public:
    using difference_type = int;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::output_iterator_tag;

  private:
		const T* init;
		const T* ptr;
		const_iterator(const T* init_, const T* ptr_) : init(init_), ptr(ptr_) {}
  public:
		const_iterator operator+(const int &n) const
    {
      return {init, ptr + n};
    }
    const_iterator operator-(const int &n) const
    {
      return {init, ptr - n};
    }
    int operator-(const const_iterator &rhs) const
    {
       if (init != rhs.init) throw invalid_iterator();
			 return ptr - rhs.ptr;
    }
    const_iterator& operator+=(const int &n)
    {
      ptr += n;
			return *this;
    }
    const_iterator& operator-=(const int &n)
    {
      ptr -= n;
			return *this;
    }
    const_iterator operator++(int) {
			return {init, ptr++};
		}
    const_iterator& operator++() {
			ptr++;
			return *this;
		}
    const_iterator operator--(int) {
			return {init, ptr--};
		}
    const_iterator& operator--() {
			ptr--;
			return *this;
		}
    const T& operator*() const { return *ptr; }
    bool operator==(const iterator &rhs) const { return ptr == rhs.ptr; }
    bool operator==(const const_iterator &rhs) const { return ptr == rhs.ptr; }
    bool operator!=(const iterator &rhs) const { return ptr != rhs.ptr; }
    bool operator!=(const const_iterator &rhs) const { return ptr != rhs.ptr; }
  };
  vector() : size_(0), capacity_(kSMALL), arr(alloc.allocate(kSMALL)) {}
  vector(const vector &other) : size_(other.size_), capacity_(other.capacity_) {
    arr = alloc.allocate(capacity_);
    for (size_t i = 0; i < size_; i++) {
      std::construct_at(arr + i, other.arr[i]);
    }
  }
  vector(vector &&other) : size_(other.size_), 
      capacity_(other.capacity_), arr(other.arr) {
    other.size_ = 0;
    other.capacity_ = kSMALL;
    other.arr = other.alloc.allocate(kSMALL);
  }
  ~vector() {
    for(size_t i = 0; i < size_; i++) {
      std::destroy_at(arr + i);
    }
    alloc.deallocate(arr, capacity_);
  }
  vector &operator=(const vector &other) {
		if (this == &other) return *this;
    for(size_t i = 0; i < size_; i++) {
      std::destroy_at(arr + i);
    }
    alloc.deallocate(arr, capacity_);
    arr = alloc.allocate(capacity_ = other.capacity_);
    size_ = other.size_;
    for (size_t i = 0; i < size_; i++) {
      std::construct_at(arr + i, other.arr[i]);
    }
		return *this;
  }
  vector &operator=(vector &&other) {
    size_ = other.size_;
    capacity_ = other.capacity_;
    arr = other.arr;
    other.size_ = 0;
    other.capacity_ = kSMALL;
    other.arr = other.alloc.allocate(kSMALL);
    return *this;
  }
  /**
   * assigns specified element with bounds checking
   * throw index_out_of_bound if pos is not in [0, size)
   * !!! Pay attentions
   *   In STL this operator does not check the boundary but I want you to do.
   */
	T & operator[](const size_t &pos) {
		if (pos >= size_) throw index_out_of_bound();
		return arr[pos];
	}
	const T & operator[](const size_t &pos) const {
		if (pos >= size_) throw index_out_of_bound();
		return arr[pos];
	}
  /**
   * assigns specified element with bounds checking
   * throw index_out_of_bound if pos is not in [0, size)
   */
	T & at(const size_t &pos) {
		if (pos >= size_) throw index_out_of_bound();
		return arr[pos];
	}
	const T & at(const size_t &pos) const {
		if (pos >= size_) throw index_out_of_bound();
		return arr[pos];
	}

  /**
   * access the first element.
   * throw container_is_empty if size == 0
   */
  const T & front() const {
		if (size_ == 0) throw container_is_empty();
		return arr[0];
	}
  /**
   * access the last element.
   * throw container_is_empty if size == 0
   */
  const T & back() const {
		if (size_ == 0) throw container_is_empty();
		return arr[size_ - 1];
	}
  /**
   * returns an iterator to the beginning.
   */
  iterator begin() {
		return {arr, arr};
	}
  const_iterator begin() const {
		return {arr, arr};
	}
  const_iterator cbegin() const {
		return {arr, arr};
	}
  /**
   * returns an iterator to the end.
   */
  iterator end() {
		return {arr, arr + size_};
	}
  const_iterator end() const {
		return {arr, arr + size_};
	}
  const_iterator cend() const {
		return {arr, arr + size_};
	}
  /**
   * checks whether the container is empty
   */
  bool empty() const { return size_ == 0; }
  /**
   * returns the number of elements
   */
  size_t size() const { return size_; }
  /**
   * clears the contents
   */
  void clear() {
    for(size_t i = 0; i < size_; i++) {
      std::destroy_at(arr + i);
    }
    alloc.deallocate(arr, capacity_);
    arr = alloc.allocate(capacity_ = kSMALL);
    size_ = 0;
  }
  /**
   * inserts value before pos
   * returns an iterator pointing to the inserted value.
   */
  iterator insert(iterator pos, const T &value) {
		size_t ind = pos.ptr - arr;
		try_double();
    std::construct_at(arr + size_, value);
		for (size_t i = size_; i > ind; i--) {
			arr[i] = std::move(arr[i - 1]);
		}
		arr[ind] = value;
		size_++;
		return {arr, arr + ind};
	}
  /**
   * inserts value at index ind.
   * after inserting, this->at(ind) == value
   * returns an iterator pointing to the inserted value.
   * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
   */
  iterator insert(const size_t &ind, const T &value) {
		if (ind > size_) throw index_out_of_bound();
		try_double();
    std::construct_at(arr + size_, value);
		for (size_t i = size_; i > ind; i--) {
			arr[i] = std::move(arr[i - 1]);
		}
		arr[ind] = value;
		size_++;
		return {arr, arr + ind};
	}
  /**
   * removes the element at pos.
   * return an iterator pointing to the following element.
   * If the iterator pos refers the last element, the end() iterator is returned.
   */
  iterator erase(iterator pos) {
		size_t ind = pos.ptr - arr;
		for (size_t i = ind; i + 1 < size_; i++) {
			arr[i] = std::move(arr[i + 1]);
		}
		size_--;
    std::destroy_at(arr + size_);
		try_shrink();
		return {arr, arr + ind};
	}
  /**
   * removes the element with index ind.
   * return an iterator pointing to the following element.
   * throw index_out_of_bound if ind >= size
   */
  iterator erase(const size_t &ind) {
		if (ind >= size_) throw index_out_of_bound();
		for (size_t i = ind; i + 1 < size_; i++) {
			arr[i] = std::move(arr[i + 1]);
		}
		size_--;
    std::destroy_at(arr + size_);
		try_shrink();
		return {arr, arr + ind};
	}
  /**
   * adds an element to the end.
   */
  void push_back(const T &value) {
		try_double();
		std::construct_at(arr + size_, value);
		size_++;
	}
  /**
   * remove the last element from the end.
   * throw container_is_empty if size() == 0
   */
  void pop_back() {
    if (size_ == 0) {
      throw container_is_empty();
    }
    size_--;
    std::destroy_at(arr + size_);
    try_shrink();
  }
  void resize(size_t siz) {
    while (size_ < siz) {
      push_back(T());
    }
    while (size_ > siz) {
      pop_back();
    }
  }
};
}

#endif