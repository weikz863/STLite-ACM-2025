#pragma once

#ifndef BPT_FILE_
#define BPT_FILE_

#include <fstream>
#include <memory>
#include "vector.hpp"
using std::ifstream, std::ofstream, std::fstream;
using sjtu::vector;

class BasicStorage {
 protected:
  bool initialized_;
 public:
  BasicStorage() = delete;
  BasicStorage(const char *name) : initialized_(false) {}
  BasicStorage(const BasicStorage &other) = default;
  BasicStorage(BasicStorage &&other) = default;
  ~BasicStorage() = default;
  BasicStorage& operator = (const BasicStorage &) = delete;
  BasicStorage& operator = (BasicStorage &&) = delete;
  virtual void write(int place, const char *value, size_t bytes) = 0;
  virtual void read(int place, char *value, size_t bytes) = 0;
  virtual int file_size() = 0;
  template<typename T> requires std::is_trivially_copyable<T>::value
  void write_at(int place, const T& value) {
    write(place, reinterpret_cast<const char*>(&value), sizeof(T));
  }
  template<typename T> requires std::is_trivially_copyable<T>::value
  void read_at(int place, T& value) {
    read(place, reinterpret_cast<char*>(&value), sizeof(T));
  }
  template<typename T, std::size_t N> requires std::is_trivially_copyable<T>::value
  void write_at(int place, const T (&value)[N]) {
    write(place, reinterpret_cast<const char*>(value), sizeof(value));
  }
  template<typename T, std::size_t N> requires std::is_trivially_copyable<T>::value
  void read_at(int place, T (&value)[N]) {
    read(place, reinterpret_cast<char*>(value), sizeof(value));
  }
  bool& initialized() { return initialized_; }
};

class VectorStorage : public BasicStorage {
 private:
  std::shared_ptr<vector<char>> data;
 public:
  VectorStorage(const char *name) : BasicStorage(name), data(std::make_shared<vector<char>>()) {}
  VectorStorage(const VectorStorage &other) = default;
  VectorStorage(VectorStorage &&other) = default;
  void write(int place, const char *value, size_t bytes) override {
    if (data->size() < place + bytes) {
      data->resize(place + bytes);
    }
    for (int i = 0; i < bytes; i++) {
      data->operator[](place + i) = value[i];
    }
  }
  void read(int place, char *value, size_t bytes) override {
    for (int i = 0; i < bytes; i++) {
      value[i] = data->operator[](place + i);
    }
  }
  int file_size() override {
    return data->size();
  }
};

class FileStorage : public BasicStorage {
 private:
  std::shared_ptr<fstream> file;
 public:
  FileStorage(const char *name) : BasicStorage(name), file(new fstream) {
    file->open(name, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
    if (file->is_open()) {
      initialized_ = true;
    } else {
      file->open(name, std::ios_base::in | std::ios_base::out | std::ios_base::trunc | std::ios::binary);
    }
  }
  FileStorage(const FileStorage &) = default;
  FileStorage(FileStorage &&) = default;
  ~FileStorage() = default;
  void write(int place, const char *value, size_t bytes) override {
    file->seekp(place);
    file->write(value, bytes);
  }
  void read(int place, char *value, size_t bytes) override {
    file->seekp(place);
    file->read(value, bytes);
  }
  int file_size() override {
    file->seekp(0, std::ios_base::end);
    return file->tellp();
  }
};

#endif