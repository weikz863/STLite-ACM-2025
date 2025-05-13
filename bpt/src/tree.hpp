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

template<typename Data, size_t block_size, typename Storage = FileStorage>
requires (std::is_base_of<BasicStorage, Storage>::value && !is_sjtu_pair_with_int<Data>::value)
class BlockBlockList {
 private:
  static int const HEAD_ROOT = BlockList<Data, block_size, Storage>::ROOT_SIZE;
  Storage storage_handler; // definition order matters here
  BlockList<Data, block_size, Storage> leaves;
  BlockList<sjtu::pair<Data, int>, block_size, Storage> heads;
 public:
  BlockBlockList(const char *str) : storage_handler(str),
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
    throw sjtu::runtime_error();
    int place = heads.find_block(x);
    typename decltype(heads)::AutonomousBlock(storage_handler, place).erase(x);
  }
};

#endif