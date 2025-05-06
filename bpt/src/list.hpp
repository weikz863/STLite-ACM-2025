#pragma once

#ifndef BPT_LIST_
#define BPT_LIST_

#include "file.hpp"
#include "vector.hpp"
#include "utility.hpp"
#include "exceptions.hpp"
#include <iostream>
#include <cstddef>
#include <functional>

using sjtu::vector;

template<typename Data, int block_size, typename Storage> 
requires std::is_base_of<BasicStorage, Storage>::value
class BlockList {
  using RawData = decltype(
    [] () {
      if constexpr (is_sjtu_pair_with_int<Data>::value) {
        return std::type_identity<typename Data::FirstType>{};
      } else {
        return std::type_identity<Data>{};
      }
    } ()
  )::type;
  using ParentDataType = sjtu::pair<RawData, int>;
  using ParentType = BlockList<ParentDataType, block_size, Storage>;
  Storage storage_handler;
  int root;
  struct BlockHead {
    int next;
    RawData first;
  };
  struct Block {
    int next;
    Data data[block_size];
    int prev, size;
    Block(int next_ = 0, int prev_ = 0, int size_ = 0) : 
        next(next_), prev(prev_), size(size_), data{} {}
    RawData& operator[] (int x) {
      if constexpr (is_sjtu_pair_with_int<Data>::value) {
        return data[x].first;
      } else {
        return data[x];
      }
    }
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
    void erase(const RawData &x) {
      for (int i = 0; i < this->size; i++) {
        if (x < this->operator[](i)) return;
        else if (!(this->operator[](i) < x)) {
          this->size--;
          for (int j = i; j < this->size; j++) {
            this->operator[](j) = this->operator[](j + 1);
          }
          return;
        }
      }
      return;
    }
  };
  static_assert(offsetof(Block, next) == 0, "Unexpected alignment in BlockList");
  static_assert(offsetof(BlockHead, first) == offsetof(Block, data), "Unexpected alignment in BlockList");
  static_assert(offsetof(ParentDataType, first) == 0, "Unexpected alignment in sjtu::pair");
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
  static const int remaining_num = block_size * 2 / 3;
public:
  struct AutonomousBlock {
    Storage storage_handler;
    int const place;
    Block block;
    AutonomousBlock(Storage &other, int place_) : storage_handler(other), place(place_) {
      if (place_ == 0) throw sjtu::runtime_error();
      storage_handler.read_at(place, block);
    }
    AutonomousBlock(const AutonomousBlock &) = delete;
    AutonomousBlock(AutonomousBlock &&) = delete;
    AutonomousBlock& operator = (const AutonomousBlock &) = delete;
    AutonomousBlock& operator = (AutonomousBlock &&) = delete;
    ~AutonomousBlock() {
      storage_handler.write_at(place, block);
    }
    auto find(const RawData &first, const RawData &last) {
      if constexpr (is_sjtu_pair_with_int<Data>::value) {
        vector<int> ret;
        for (int i = 0; i < block.size; i++) {
          if (block.data[i].first < first) continue;
          if (last < block.data[i].first) return ret;
          ret.push_back(block.data[i].second);
        }
        return ret;
      } else {
        vector<RawData> ret;
        for (int i = 0; i < block.size; i++) {
          if (block.data[i] < first) continue;
          if (last < block.data[i]) return ret;
          ret.push_back(block.data[i]);
        }
        return ret;
      }
    }
    AccumulativeFunc<typename ParentType::AutonomousBlock> erase(const RawData &x) {
      block.erase(x);
      if (block.size == 0) {
        storage_handler.write_at(block.prev, block.next);
        if (block.next) {
          storage_handler.write_at(block.next, block.prev);
        }
      }
      return {};
    }
  };
  static const int ROOT_SIZE = sizeof(root);
  template<typename... Args>
  BlockList (Args... args) : BlockList(0, args...) {}
  template<typename... Args>
  BlockList (int root_, Args... args) : root(root_),
      storage_handler(std::forward<Args...>(args...)) {
    if (storage_handler.file_size() > root) {
      ;
    } else {
      storage_handler.template write_at<decltype(root)>(root, 0);
    }
  }
  int operator&() const {
    return root;
  }
  vector<RawData> find(const RawData &begin, const RawData &end, int head_ptr_place) {
    vector<RawData> ret{};
    BlockHead head;
    Block block;
    int current_block;
    storage_handler.read_at(head_ptr_place, current_block);
    int next_block = current_block;
    while (next_block) {
      storage_handler.read_at(next_block, head);
      if (begin < head.first) break;
      current_block = next_block;
      next_block = head.next;
    }
    while (current_block) {
      storage_handler.read_at(current_block, head);
      if (end < head.first) break;
      auto ans = AutonomousBlock(storage_handler, current_block).find(begin, end);
      for (const auto &t : ans) {
        ret.push_back(t);
      }
      current_block = block.next;
    }
    return ret;
  }
  void insert(const Data &x, int chain_head) {
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
  AccumulativeFunc<typename ParentType::AutonomousBlock> erase(const RawData &x, int head_ptr_place) {
    BlockHead head;
    Block block;
    int current_block;
    storage_handler.read_at(head_ptr_place, current_block);
    if (!current_block) return {};
    int next_block = current_block;
    while (next_block) {
      storage_handler.read_at(next_block, head);
      if (x < head.first) break;
      current_block = next_block;
      next_block = head.next;
    }
    return AutonomousBlock(storage_handler, current_block).erase(x);
  }
  vector<RawData> find(const RawData &begin, const RawData &end) {
    return find(begin, end, root);
  }
  void insert(const Data &x) {
    insert(x, root);
  }
  AccumulativeFunc<typename ParentType::AutonomousBlock> erase(const Data &x) {
    return erase(x, root);
  }
};


#endif