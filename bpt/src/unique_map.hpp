#pragma once

#ifndef _UNIQUE_MAP_
#define _UNIQUE_MAP_

#include "blockblocklist.hpp"
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
    ReferenceType(ReferenceType&&) = delete;
    ReferenceType& operator = (const ReferenceType&) = delete;
    ReferenceType& operator = (ReferenceType&&) = delete;
    operator T& () {
      if (!place) throw sjtu::runtime_error();
      return value;
    }
    operator const T& () const {
      if (!place) throw sjtu::runtime_error();
      return value;
    }
    bool empty() const {
      return !place;
    }
    ~ReferenceType() {
      if (place) file.write_at(place, value);
    }
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
  ReferenceType operator [] (int x) {
    if (x < 0 || x >= size_) return {0, file};
    return {sizeof(int) + x * sizeof(T), file};
  }
  void push_back(const T& x) {
    file.write_at(file.file_size(), x);
    size_++;
  }
  int size() const { return size_; }
};

template<typename Key, typename Value>
class UniqueMap {
 private:
  BlockBlockList<std::pair<Key, int>, 4096 / sizeof(std::pair<Key, int>)> map1;
  FileVector<Value> map2;
 public:
  UniqueMap(const string& s) : map1(s + "_map1"), map2(s + "_map2") {}
  void insert(const Key& key, const Value& value) {
    map1.insert(std::make_pair(key, map2.size()));
    map2.push_back(value);
  }
  FileVector<Value>::ReferenceType operator [] (const Key& key) {
    auto tmp = map1.find(std::make_pair(key, 0), std::make_pair(key, INT_MAX));
    if (tmp.size()) return map2[tmp.front().second];
    else return map2[-1];
  }
  int size() const {
    return map2.size();
  }
};

#endif