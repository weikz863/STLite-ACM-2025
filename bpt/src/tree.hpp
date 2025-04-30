#pragma once

#ifndef BPT_TREE_
#define BPT_TREE_

#include "file.hpp"

template<typename Index, typename Value, size_t block_size, typename Storage = VectorStorage>
requires std::is_base_of<BasicStorage, Storage>::value
class BPlusTree {
 private:
  Storage storage_handler;
  struct Block {
    int size;
    Index ind[block_size];
    int pointer[block_size]; // positive to child block, nonpositive to list of values
    Block() : size(0), ind{}, pointer{} {}
  };
  inline constexpr int nth_block(int i) {
    return i * sizeof(Block);
  }
 public:
  BPlusTree() : storage_handler("tree") {
    if (!storage_handler.initialized()) {
      storage_handler.write_at(0, Block());
    }
  }
  vector<Value> find(const Index &ind, int block_num = 0) {
    Block block;
    storage_handler.read_at(nth_block(block_num), block);
    if (block.pointer[0] > 0) {
      for (int i = 0; i < block.size; i++) {
        if (!(ind < block.ind[i])) {
          return find(ind, block.pointer[i]);
        }
      }
    } else {
      for (int i = 0; i < block.size; i++) {
        if (!(ind < block.ind[i])) {
          if (!(block.ind[i] < ind)) {
            // return list.get(-block.pointer[i]);
          } else {
            return vector<Value>();
          }
        }
      }
    }
    return vector<Value>();
  }
};

#endif