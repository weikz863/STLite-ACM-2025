#pragma once

#ifndef BPT_TREE_
#define BPT_TREE_

#include <string>
#include "file.hpp"
#include "list.hpp"

template<typename Data, size_t block_size, typename Storage = FileStorage>
requires std::is_base_of<BasicStorage, Storage>::value
class BPlusTree {
 private:
  Storage storage_handler; // this should be defined before others to ensure correct initialization sequence
  BlockList<Data, block_size, Storage> leaves;
 public:
  BPlusTree(const char *str) : storage_handler(str), leaves(storage_handler) {}
};

#endif