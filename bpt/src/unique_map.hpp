#pragma once

#ifndef _UNIQUE_MAP_
#define _UNIQUE_MAP_

#include "blockblocklist.hpp"
#include "utility.hpp"
#include <string>
#include <string_view>
using std::string, std::string_view;

template<typename T>
class FileVector {
 private:
  FileStorage file;
  int size_;
 public:
  class ReferenceType {
   private:
    T value;
    int place;
    FileStorage file;
    ReferenceType(int p, FileStorage f) : place(p), file(f) {
      if (place) file.read_at(place, value);
    }
   public:
    ReferenceType(const ReferenceType&) = delete;
    ReferenceType(ReferenceType&& other) : value(other.value), 
        place(other.place), file(other.file) {
      other.place = 0;
    }
    ReferenceType& operator = (const ReferenceType&) = delete;
    ReferenceType& operator = (ReferenceType&& other) {
      if (place) file.write_at(place, value);
      value = other.value;
      place = other.place;
      file = other.file;
      other.place = 0;
    }
    T& operator * () {
      if (!place) throw sjtu::runtime_error();
      return value;
    }
    const T& operator * () const {
      if (!place) throw sjtu::runtime_error();
      return value;
    }
    T* operator -> () {
      if (!place) throw sjtu::runtime_error();
      return &value;
    }
    const T* operator -> () const {
      if (!place) throw sjtu::runtime_error();
      return &value;
    }
    bool empty() const {
      return !place;
    }
    int file_pos() const {
      return place;
    }
    ~ReferenceType() {
      if (place) file.write_at(place, value);
    }
    friend FileVector;
  };
  FileVector(const string_view str) : file(str.data()) {
    if (file.file_size() == 0) {
      file.template write_at<int>(0, 0);
      size_ = 0;
    } else {
      size_ = (file.file_size() - sizeof(int)) / sizeof(T);
    }
  }
  FileVector(const FileVector&) = delete;
  FileVector(FileVector&&) = delete;
  FileVector& operator = (const FileVector&) = delete;
  FileVector& operator = (FileVector&&) = delete;
  ~FileVector() {
    file.write_at(0, size_);
  }
  ReferenceType operator [] (int x) {
    if (x < 0 || x >= size_) return {0, file};
    return {static_cast<int>(sizeof(int) + x * sizeof(T)), file};
  }
  ReferenceType make_reference(int x) {
    return {x, file};
  }
  void push_back(const T& x) {
    file.write_at(file.file_size(), x);
    size_++;
  }
  int size() const { return size_; }
};

template<typename Key, typename Value>
requires (std::is_trivially_copyable<Key>::value && std::is_trivially_copyable<Value>::value)
class UniqueMap {
 private:
  BlockBlockList<trivial_pair<Key, int>, 4096 / sizeof(trivial_pair<Key, int>)> map1;
  FileVector<Value> map2;
 public:
  using ReferenceType = FileVector<Value>::ReferenceType;
  UniqueMap(const string& s) : map1(s + "_map1"), map2(s + "_map2") {}
  void insert(const Key& key, const Value& value) {
    map1.insert(make_trivial_pair(key, map2.size()));
    map2.push_back(value);
  }
  ReferenceType operator [] (const Key& key) {
    auto tmp = map1.find(make_trivial_pair(key, 0), make_trivial_pair(key, INT_MAX));
    if (tmp.size() > 1) throw sjtu::runtime_error();
    if (tmp.size()) return map2[tmp.front().second];
    else return map2[-1];
  }
  ReferenceType make_reference(int x) {
    return map2.make_reference(x);
  }
  bool erase(const Key& key) {
    auto tmp = map1.find(make_trivial_pair(key, 0), make_trivial_pair(key, INT_MAX));
    if (tmp.size() > 1) throw sjtu::runtime_error();
    if (tmp.size()) {
      map1.erase(tmp[0]);
      return true;
    } else {
      return false;
    }
  }
  int size() const {
    return map2.size();
  }
};

#endif