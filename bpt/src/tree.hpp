#pragma once

#ifndef BPT_TREE_
#define BPT_TREE_

#include <string>
#include <iostream>
#include "file.hpp"
#include "list.hpp"
#include "utility.hpp"

template<typename Data, size_t block_size, typename Storage = FileStorage>
requires (std::is_base_of<BasicStorage, Storage>::value && !is_sjtu_pair_with_int<Data>::value)
class BPlusTree {
 private:
  Storage storage_handler; // this should be defined before others to ensure correct initialization sequence
  BlockList<Data, block_size, Storage> leaves;
  int root;
 public:
  BPlusTree(const char *str) : storage_handler(str), leaves(storage_handler), root(0) {}
  vector<Data> find(const Data &begin, const Data &end, int list_root, int l = 0, int r = 0) {
    if (list_root == &leaves) {
      return leaves.find(begin, end);
    }
  }
  void insert(const Data &x, int list_root, int l = 0, int r = 0) {
    if (list_root == &leaves) {
      leaves.insert(x);
      return;
    }
  }
  void erase(const Data &x, int list_root, int l = 0, int r = 0) {
    if (list_root == &leaves) {
      leaves.erase(x);
      return;
    }
  }
  vector<Data> find(const Data &begin, const Data &end) {
    return find(begin, end, root);
  }
  void insert(const Data &x) {
    insert(x, root);
  }
  void erase(const Data &x) {
    erase(x, root);
  }
};

#endif