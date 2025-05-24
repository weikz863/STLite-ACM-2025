#pragma once

#ifndef BPT_BBL_
#define BPT_BBL_

#include <string>
#include <iostream>
#include "file.hpp"
#include "list_bbl.hpp"
#include "utility.hpp"

template<typename Data, size_t block_size, typename Storage = FileStorage>
requires (std::is_base_of<BasicStorage, Storage>::value && !is_sjtu_pair_with_int<Data>::value)
class BlockBlockList {
 private:
  static int const HEAD_ROOT = BlockList<Data, block_size, Storage>::ROOT_SIZE;
  struct InitializeHelper {
    InitializeHelper(Storage &storage) {
      storage.template write_at<int>(0, 0);
    }
  };
  Storage storage_handler; // definition order matters here
  InitializeHelper helper;
  BlockList<Data, block_size, Storage> leaves;
  BlockList<sjtu::pair<Data, int>, block_size, Storage> heads;
 public:
  BlockBlockList(const char *str) : storage_handler(str), helper(storage_handler),
      leaves(HEAD_ROOT, storage_handler), heads(2 * HEAD_ROOT, storage_handler) {}
  vector<Data> find(const Data &begin, const Data &end) {
    // std::cerr << "BBL::FIND\n";
    int place = heads.find_block(begin);
    if (place == 0) return {};
    return leaves.find(begin, end, 
      typename decltype(heads)::AutonomousBlock(storage_handler, place).find(begin));
  }
  void insert(const Data &x) {
    // std::cerr << "BBL::INSERT\n";
    int place = heads.find_block(x);
    if (place == 0) {
      // std::cerr << "BBL::INSERT::place == 0\n";
      leaves.insert(x);
      int block_place;
      storage_handler.read_at(&leaves, block_place);
      heads.insert(sjtu::pair<Data, int>(x, block_place));
      return;
    }
    // std::cerr << "BBL::INSERT::place != 0\n";
    typename decltype(heads)::AutonomousBlock(storage_handler, place).insert(x);
  }
  void erase(const Data &x) {
    int place = heads.find_block(x);
    if (place == 0) return;
    typename decltype(heads)::AutonomousBlock(storage_handler, place).erase(x);
  }
};

#endif