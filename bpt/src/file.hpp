#pragma once

#ifndef BPT_
#define BPT_

#include <fstream>
#include "vector.hpp"
using std::ifstream, std::ofstream, std::fstream;
using sjtu::vector;

class BasicStorage {
 public:
  BasicStorage() = delete;
  BasicStorage(const char *name) {}
  BasicStorage(const BasicStorage &) = delete;
  BasicStorage(BasicStorage &&) = delete;
  BasicStorage& operator = (const BasicStorage &) = delete;
  BasicStorage& operator = (BasicStorage &&) = delete;
  virtual void write(int place, const char *value, size_t bytes) = 0;
  virtual void read(int place, char *value, size_t bytes) = 0;
  template<typename T>
  void write_at(int place, const T& value) {
    write(place, reinterpret_cast<const char*>(&value), sizeof(T));
  }
  template<typename T>
  void read_at(int place, T& value) {
    read(place, reinterpret_cast<char*>(&value), sizeof(T));
  }
  template<typename T, std::size_t N>
  void write_at(int place, const T (&value)[N]) {
    write(place, reinterpret_cast<const char*>(value), sizeof(value));
  }
  template<typename T, std::size_t N>
  void read_at(int place, T (&value)[N]) {
    read(place, reinterpret_cast<char*>(value), sizeof(value));
  }
};

class VectorStorage : public BasicStorage {
 private:
  vector<char> data;
 public:
  VectorStorage(const char *name) : BasicStorage(name), data() {}
  void write(int place, const char *value, size_t bytes) override {
    if (data.size() < place + bytes) {
      data.resize(place + bytes);
    }
    for (int i = 0; i < bytes; i++) {
      data[place + i] = value[i];
    }
  }
  void read(int place, char *value, size_t bytes) override {
    for (int i = 0; i < bytes; i++) {
      value[i] = data[place + i];
    }
  }
};

#endif