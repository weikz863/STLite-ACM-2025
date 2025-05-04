#pragma once

#ifndef BPT_LIST_
#define BPT_LIST_

#include "file.hpp"
#include "vector.hpp"
#include "exceptions.hpp"
#include <iostream>
#include <cstddef>

using sjtu::vector;

template<typename Data, int block_size, typename Storage> 
requires std::is_base_of<BasicStorage, Storage>::value
class BlockList {
  static const int remaining_num = block_size * 2 / 3;
  Storage storage_handler;
  struct BlockHead {
    int next;
    Data first;
  };
  struct Block {
    int next;
    Data data[block_size];
    int prev, size;
    Block(int next_ = 0, int prev_ = 0, int size_ = 0) : 
        next(next_), prev(prev_), size(size_), data{} {}
    void insert(const Data &x) {
      if (this->size == block_size) throw sjtu::runtime_error();
      this->data[this->size] = x;
      this->size++;
      for (int i = this->size - 2; i >= 0; i--) {
        if (x < this->data[i]) {
          std::swap(this->data[i], this->data[i + 1]);
        } else {
          return;
        }
      }
    }
    bool erase(const Data &x) {
      for (int i = 0; i < this->size; i++) {
        if (x < this->data[i]) return false;
        else if (!(this->data[i] < x)) {
          this->size--;
          for (int j = i; j < this->size; j++) {
            this->data[j] = this->data[j + 1];
          }
          return true;
        }
      }
      return false;
    }
  };
  static_assert(offsetof(Block, next) == 0, "Unexpected alignment in BlockList");
  static_assert(offsetof(BlockHead, first) == offsetof(Block, data), "Unexpected alignment in BlockList");
  void new_block(const Block& block) {
    int place = storage_handler.file_size();
    storage_handler.write_at(place, block);
    if (block.next != 0) {
      storage_handler.write_at(block.next + offsetof(Block, prev), place);
    }
    storage_handler.write_at(block.prev, place);
  }
  void erase_block(const int prev, const int next) {
    if (next != 0) {
      storage_handler.write_at(next + offsetof(Block, prev), prev);
    }
    storage_handler.write_at(prev, next);
  }

public:
  template<typename... Args>
  BlockList (Args... args) : storage_handler(std::forward<Args...>(args...)) {
    if (!storage_handler.initialized() && new_chain() != 0) {
      throw sjtu::runtime_error();
    }
  }
  int new_chain() {
    int place = storage_handler.file_size();
    storage_handler.template write_at<int>(place, 0); // what the heck is this?
    return place;
  }
  vector<Data> find(const Data &begin, const Data &end, int chain_head = 0) {
    vector<Data> ret{};
    BlockHead head;
    Block block;
    int current_block;
    storage_handler.read_at(chain_head, current_block);
    int next_block = current_block;
    while (next_block) {
      storage_handler.read_at(next_block, head);
      if (begin < head.first) break;
      current_block = next_block;
      next_block = head.next;
    }
    while (current_block) {
      storage_handler.read_at(current_block, block);
      for (int i = 0; i < block.size; i++) {
        if (block.data[i] < begin) continue;
        if (end < block.data[i]) return ret;
        ret.push_back(block.data[i]);
      }
      current_block = block.next;
    }
    return ret;
  }
  void insert(const Data &x, int chain_head = 0) {
    BlockHead head;
    Block block;
    int current_block;
    storage_handler.read_at(chain_head, current_block);
    if (!current_block) {
      block.prev = chain_head;
      block.data[0] = x;
      block.size = 1;
      new_block(block);
      return;
    }
    int next_block = current_block;
    while (next_block) {
      storage_handler.read_at(next_block, head);
      if (x < head.first) break;
      current_block = next_block;
      next_block = head.next;
    }
    storage_handler.read_at(current_block, block);
    if (block.size == block_size) {
      block.size = remaining_num;
      Block block_after(block.next, current_block, block_size - remaining_num);
      for (int i = remaining_num; i < block_size; i++) {
        block_after.data[i - remaining_num] = block.data[i];
      }
      if (x < block_after.data[0]) {
        block.insert(x);
      } else {
        block_after.insert(x);
      }
      storage_handler.write_at(current_block, block);
      new_block(block_after);
    } else {
      block.insert(x);
      storage_handler.write_at(current_block, block);
    }
  }
  void erase(const Data &x, int chain_head = 0) {
    BlockHead head;
    Block block;
    int current_block;
    storage_handler.read_at(chain_head, current_block);
    if (!current_block) return;
    int next_block = current_block;
    while (next_block) {
      storage_handler.read_at(next_block, head);
      if (x < head.first) break;
      current_block = next_block;
      next_block = head.next;
    }
    storage_handler.read_at(current_block, block);
    if (block.erase(x)) {
      if (block.next) {
        int next_size = 0;
        storage_handler.read_at(block.next + offsetof(Block, size), next_size);
        if (block.size + next_size <= block_size / 2) {
          Block next;
          storage_handler.read_at(block.next, next);
          for (int i = 0; i < next.size; i++) {
            block.data[i + block.size] = next.data[i];
          }
          block.size += next.size;
          block.next = next.next;
          if (block.next) {
            storage_handler.write_at(block.next + offsetof(Block, prev), current_block);
          }
        }
      }
      storage_handler.write_at(current_block, block);
    }
  }
};


#endif