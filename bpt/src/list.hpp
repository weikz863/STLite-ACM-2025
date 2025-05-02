#pragma once

#ifndef BPT_LIST_
#define BPT_LIST_

#include "file.hpp"
#include "vector.hpp"
#include <cstddef>

using sjtu::vector;

template<typename Data, int block_size, typename Storage> 
requires std::is_base_of<BasicStorage, Storage>::value
class BlockList {
  static constexpr int ADDITIONAL = 2 * sizeof(int);
  Storage storage_handler;
  int block_cnt, first_block;
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
  };
  static_assert(offsetof(BlockHead, first) == offsetof(Block, data), "Unexpected alignment in BlockList");
  inline constexpr int nth_block(int x) {
    return ADDITIONAL + sizeof(Block) * x;
  }
  void newblock(const Block& block) {
    int place = nth_block(block_cnt);
    storage_handler.write_at(place, block);
    block_cnt++;
    storage_handler.write_at(0, block_cnt);
    if (block.next != 0) {
      storage_handler.write_at(block.next + offsetof(Block, prev), place);
    }
    if (block.prev != 0) {
      storage_handler.write_at(block.prev + offsetof(Block, next), place);
    } else {
      storage_handler.write_at(sizeof(int), place);
    }
  }
  void delblock(const int place) {
    
  }

public:
  BlockList (const char *s) : storage_handler(s), block_cnt(0), first_block(0) {
    if (storage_handler.initialized()) {
      storage_handler.read_at(0, block_cnt);
      storage_handler.read_at(sizeof(int), first_block);
    } else {
      storage_handler.write_at(0, block_cnt);
      storage_handler.write_at(sizeof(int), first_block);
    }
  };
  vector<Data> find(const Data &begin, const Data &end, int current_block) {
    vector<Data> ret{};
    BlockHead head;
    Block block;
    int next_block = current_block;
    while (next_block) {
      storage_handler.read_at(next_block, head);
      if (begin < head.data) break;
      current_block = next_block;
      next_block = head.next;
    }
    while (current_block) {
      storage_handler.read_at(current_block, block);
      for (int i = 0; i < block_size; i++) {
        if (block.data[i] < begin) continue;
        if (end < block.data[i]) return ret;
        ret.push_back(block.data[i]);
      }
    }
    return ret;
  }
  vector<Data> find(const Data &begin, const Data &end) {
    return find(begin, end, first_block);
  }
  /*
  void insert(const Data &x) {
    if (blocks.empty()) {
      newblock(0, vector<Data>{x});
      return;
    }
    int blockid = 0;
    while (blockid < blocks.size() && blocks[blockid].first <= x) {
      blockid++;
    }
    blockid--;
    if (blockid < 0) {
      blockid = 0;
    }
    int pos = 0;
    Data cur;
    file.seekp(blocks[blockid].pos + ADDITIONAL);
    cur.readfrom(file);
    pos++;
    while (pos < blocks[blockid].size && cur < x) {
      cur.readfrom(file);
      pos++;
    }
    if (cur >= x) {
      file.seekp(-sizeof(Data), std::ios_base::cur);
      x.writeto(file);
    } else {
      cur = x;
    }
    Data t;
    while (pos < blocks[blockid].size) {
      t.readfrom(file);
      file.seekp(-sizeof(Data), std::ios_base::cur);
      cur.writeto(file);
      cur = t;
      pos++;
    }
    if (blocks[blockid].size == block_size) {
      int const rem = block_size * 2 / 3;
      file.seekp(blocks[blockid].pos + rem * sizeof(Data) + ADDITIONAL);
      vector<Data> split(block_size - rem);
      for (auto &tmp : split) tmp.readfrom(file);
      split.push_back(cur);
      newblock(blockid + 1, split);
      blocks[blockid].size = rem;
    } else {
      blocks[blockid].size++;
      cur.writeto(file);
    }
    file.seekp(blocks[blockid].pos + ADDITIONAL / 2);
    file.write(reinterpret_cast<char*>(&blocks[blockid].size), sizeof(int));
    blocks[blockid].first.readfrom(file);
  }
  void delet(const Data &x) {
    int blockid = 0; 
    while (blockid < blocks.size() && blocks[blockid].first <= x) blockid++;
    blockid = std::max(blockid - 1, 0);
    Data t;
    for (int i = blockid; i < blocks.size(); i++) {
      file.seekp(blocks[i].pos + ADDITIONAL);
      for (int j = 0; j < blocks[i].size; j++) {
        t.readfrom(file);
        if (t > x) return;
        else if (t == x) {
          int pos = int(file.tellg()) - sizeof(Data);
          vector<Data> tmp(blocks[i].size - j - 1);
          for (auto &dat : tmp) dat.readfrom(file);
          file.seekg(pos);
          for (auto &dat : tmp) dat.writeto(file);
          blocks[i].size--;
          if (blocks[i].size <= 0) {
            delblock(i);
          } else {
            file.seekg(blocks[i].pos + ADDITIONAL / 2);
            file.write(reinterpret_cast<char*>(&blocks[i].size), sizeof(int));
            blocks[i].first.readfrom(file);
          }
          return;
        }
      }
    }
  }
    */
};


#endif