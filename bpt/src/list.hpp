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
    static const int remaining_num = block_size * 2 / 3;
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
    const RawData& operator[] (int x) const {
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
public:
  struct AutonomousBlock {
    Storage storage_handler;
    int const place;
    bool changed;
    Block block;
    AutonomousBlock(Storage &other, int place_) : storage_handler(other), place(place_), changed(false) {
      if (place == 0) throw sjtu::runtime_error();
      storage_handler.read_at(place, block);
    }
    AutonomousBlock(const AutonomousBlock &) = delete;
    AutonomousBlock(AutonomousBlock &&) = delete;
    AutonomousBlock& operator = (const AutonomousBlock &) = delete;
    AutonomousBlock& operator = (AutonomousBlock &&) = delete;
    ~AutonomousBlock() {
      if (changed) storage_handler.write_at(place, block);
    }
    int find(const RawData &first) {
      if constexpr (is_sjtu_pair_with_int<Data>::value) {
        for (int i = 0; i < block.size; i++) {
          if (first < block.operator[](i)) {
            return AutonomousBlock(storage_handler, block.data[std::max(i - 1, 0)]).find(first);
          }
        }
        return AutonomousBlock(storage_handler, block.data[block.size - 1]).find(first);
      } else {
        return place;
      }
    }
    AccumulativeFunc<typename ParentType::AutonomousBlock> erase(const RawData &x) {
      for (int i = 0; i < block.size; i++) {
        if (x < block.operator[](i)) return {};
        else if (!(block.operator[](i) < x)) {
          changed = true;
          block.size--;
          for (int j = i; j < block.size; j++) {
            block.operator[](j) = block.operator[](j + 1);
          }
          if (block.size == 0) {
            storage_handler.write_at(block.prev, block.next);
            if (block.next) {
              storage_handler.write_at(block.next + offsetof(Block, prev), block.prev);
            }
            changed = false;
          } else if (block.next) {
            int next_size;
            storage_handler.read_at(block.next + offsetof(Block, size), next_size);
            if (block.size + next_size < block_size / 2) {
              Block next;
              storage_handler.read_at(block.next, next);
              for (int i = 0; i < next.size; i++) {
                block.data[i + block.size] = next.data[i];
              }
              block.size += next.size;
              block.next = next.next;
              if (block.next) {
                storage_handler.write_at(block.next + offsetof(Block, prev), place);
              }
            }
          }
          return {};
        }
      }
      return {};
    }
    AccumulativeFunc<typename ParentType::AutonomousBlock> insert(const RawData &x) {
      changed = true;
      if (block.size == block_size) {
        block.size = block.remaining_num;
        Block block_after(block.next, place, block_size - block.remaining_num);
        for (int i = block.remaining_num; i < block_size; i++) {
          block_after.data[i - block.remaining_num] = block.data[i];
        }
        if (x < block_after.data[0]) {
          block.insert(x);
        } else {
          block_after.insert(x);
        }
        int new_place = storage_handler.file_size();
        storage_handler.write_at(new_place, block_after);
        if (block_after.next != 0) {
          storage_handler.write_at(block_after.next + offsetof(Block, prev), new_place);
        }
        block.next = new_place;
      } else {
        block.insert(x);
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
  vector<RawData> find(const RawData &begin, const RawData &end, int current_block)
  requires (!is_sjtu_pair_with_int<Data>::value) {
    vector<RawData> ret{};
    Block block;
    while (current_block) {
      storage_handler.read_at(current_block, block);
      for (int i = 0; i < block.size; i++) {
        if (block[i] < begin) continue;
        if (end < block[i]) return ret;
        ret.push_back(block[i]);
      }
      current_block = block.next;
    }
    return ret;
  }
  int find_block(const RawData &x) {
    BlockHead head;
    int current_block;
    storage_handler.read_at(root, current_block);
    int next_block = current_block;
    while (next_block) {
      storage_handler.read_at(next_block, head);
      if (x < head.first) break;
      current_block = next_block;
      next_block = head.next;
    }
    return current_block;
  }
  vector<RawData> find(const RawData &begin, const RawData &end)
  requires (!is_sjtu_pair_with_int<Data>::value) {
    return find(begin, end, find_block(begin));
  }
  void insert(const Data &x)
  requires (!is_sjtu_pair_with_int<Data>::value) {
    int t = find_block(x);
    if (t == 0) {
      Block block(0, root, 1);
      block.data[0] = x;
      new_block(block);
    } else {
      AutonomousBlock(storage_handler, find_block(x)).insert(x);
    }
  }
  void erase(const Data &x)
  requires (!is_sjtu_pair_with_int<Data>::value) {
    AutonomousBlock(storage_handler, find_block(x)).erase(x);
  }
};


#endif