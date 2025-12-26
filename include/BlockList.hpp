#ifndef BLOCKLIST_HPP
#define BLOCKLIST_HPP

#include "MemoryRiver.hpp"
#include <cstddef>
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>

template <typename K, typename V> class BlockList
{
public:
  static constexpr int blockSize = 500;
  struct KeyValue
  {
    K key_;
    V value_;
    auto operator<=>(const KeyValue &) const = default;
  };
  struct Block
  {
    int size_;
    int pre_;
    int nxt_;
    KeyValue pairs[blockSize];
  };
  MemoryRiver<Block, 2> io_; // 第一个存count，第二个存head
  void Initialise(std::string file_name);
  void Insert(K key, V value);
  void Delete(K key, V value);
  std::vector<V> Find(K key);
  int CountAllPairs();
  std::vector<V> GetAll();
  std::vector<std::pair<K, V>> GetAllPairs();
  void SplitBlock(int index);
  void DeleteBlock(int index);                     // 将空块从链表中删掉
  void InsertBlock(int index, KeyValue key_value); // 将键值对插入index号的block中
  BlockList() = default;
  ~BlockList() = default;
};
template <typename K, typename V> void BlockList<K, V>::Insert(K key, V value)
{
  int head;
  io_.GetInfo(head, 2);
  if (head == 0) // 空链表,要创一个新块
  {
    Block new_block;
    new_block.pre_ = 0;
    new_block.nxt_ = 0;
    new_block.pairs[0] = KeyValue{.key_ = key, .value_ = value};
    new_block.size_ = 1;
    int new_head = io_.Write(new_block);
    // std::cerr << "new_head: " << new_head << std::endl;
    io_.WriteInfo(new_head, 2);
    return;
  }
  int now = head;
  KeyValue key_value{.key_ = key, .value_ = value};
  while (now != 0)
  {
    Block block;
    io_.Read(block, now);
    if (block.pairs[block.size_ - 1] > key_value) // 找到第一个最大值大于插入键值对的块
    {
      InsertBlock(now, key_value);
      if (block.size_ >= blockSize - 1) // 要分块
      {
        SplitBlock(now);
      }
      return;
    }
    if(block.nxt_ == 0)
    {
      InsertBlock(now, key_value);// 到最后一个块了必须插入了
      if (block.size_ == blockSize - 1) // 要分块
      {
        SplitBlock(now);
      }
      return;
    }
    now = block.nxt_; // now->nxt
  }
}
template <typename K, typename V> void BlockList<K, V>::Delete(K key, V value)
{
  int head;
  io_.GetInfo(head, 2);
  KeyValue key_value{.key_ = key, .value_ = value};
  int now = head;
  while (now != 0)
  {
    Block block_delete;
    io_.Read(block_delete, now);
    if (block_delete.pairs[block_delete.size_ - 1] >= key_value)
    {
      int size = block_delete.size_;
      int pos = std::lower_bound(block_delete.pairs, block_delete.pairs + size, key_value) -
                block_delete.pairs;
      if (block_delete.pairs[pos] == key_value) // 确实一样，要删
      {
        for (int i = pos; i <= size - 2; i++)
        {
          block_delete.pairs[i] = block_delete.pairs[i + 1];
        }
        block_delete.size_ -= 1;
        io_.Update(block_delete, now);
        if (block_delete.size_ == 0)
        {
          DeleteBlock(now);
        }
      }
      return;
    }
    else
    {
      now = block_delete.nxt_;
    }
  }
  return;
}
template <typename K, typename V> std::vector<V> BlockList<K, V>::Find(K key)
{
  int head;
  io_.GetInfo(head, 2);
  // std::cerr << "head: " << head << std::endl;
  std::vector<V> result;
  int now = head;
  while (now != 0)
  {
    Block block;
    io_.Read(block, now);
    if (block.pairs[block.size_ - 1].key_ >= key)
    {
      break;
    }
    now = block.nxt_;
  }
  // std::cerr << "find->now: " << now << std::endl;
  if (now == 0) // 没找到
  {
    return result;
  }
  while (now != 0)
  {
    Block block;
    io_.Read(block, now);
    if (block.pairs[0].key_ > key)
    {
      break;
    }
    // std::cerr << block.size_ << std::endl;
    for (int i = 0; i <= block.size_ - 1; i++)
    {
      if (block.pairs[i].key_ == key)
      {
        result.push_back(block.pairs[i].value_);
      }
    }
    now = block.nxt_;
  }
  return result;
}
template <typename K, typename V> void BlockList<K, V>::Initialise(std::string file_name)
{
  io_.Initialise(file_name);
}
template <typename K, typename V> int BlockList<K, V>::CountAllPairs() 
{
  int head;
  io_.GetInfo(head, 2);
  int count = 0;
  int now = head;
  while (now != 0)
  {
    Block block;
    io_.Read(block, now);
    int size = block.size_;
    count += size;
    now = block.nxt_;
  }
  return count;
}
template<typename K, typename V> std::vector<V> BlockList<K, V>::GetAll()
{
  std::vector<V> result;
  int head;
  io_.GetInfo(head, 2);
  int now = head;
  while (now != 0)
  {
    Block block;
    io_.Read(block, now);
    int size = block.size_;
    for(int i = 0; i <= size - 1; i++)
    {
      result.push_back(block.pairs[i].value_);
    }
    now = block.nxt_;
  }
  return result;
}
template<typename K, typename V> std::vector<std::pair<K, V>> BlockList<K, V>::GetAllPairs()
{
  std::vector<std::pair<K, V>> result;
  int head;
  io_.GetInfo(head, 2);
  int now = head;
  while (now != 0)
  {
    Block block;
    io_.Read(block, now);
    int size = block.size_;
    for(int i = 0; i <= size - 1; i++)
    {
      result.push_back(std::make_pair(block.pairs[i].key_, block.pairs[i].value_));
    }
    now = block.nxt_;
  }
  return result;
}
template<typename K, typename V> void BlockList<K, V>::DeleteBlock(int index)
{
  Block block;
  io_.Read(block, index);
  int pre = block.pre_;
  int nxt = block.nxt_;
  if(pre != 0)
  {
    Block pre_block;
    io_.Read(pre_block, pre);
    pre_block.nxt_ = nxt;
    io_.Update(pre_block, pre);
  }
  else
  {
    io_.WriteInfo(nxt, 2);
  }
  if(nxt != 0)
  {
    Block nxt_block;
    io_.Read(nxt_block, nxt);
    nxt_block.pre_ = pre;
    io_.Update(nxt_block, nxt);
  }
}
template<typename K, typename V> void BlockList<K, V>::SplitBlock(int index)
{
  int size1 = blockSize / 2;
  int size2 = blockSize - size1;
  Block split_block;
  io_.Read(split_block, index);
  split_block.size_ = size1;
  int old_nxt = split_block.nxt_;
  Block nxt_block;
  for(int i = 0; i <= size2 - 1; i++)
  {
    nxt_block.pairs[i] = split_block.pairs[size1 + i];
  }
  nxt_block.pre_ = index;
  nxt_block.size_ = size2;
  nxt_block.nxt_ = old_nxt;
  int nxt = io_.Write(nxt_block);
  split_block.nxt_ = nxt;
  io_.Update(split_block, index);
  if(old_nxt != 0)
  {
    Block old_nxt_block;
    io_.Read(old_nxt_block, old_nxt);
    old_nxt_block.pre_ = nxt;
    io_.Update(old_nxt_block, old_nxt);
  }
}
template<typename K, typename V> void BlockList<K, V>::InsertBlock(int index, KeyValue key_value)
{
  Block block;
  io_.Read(block, index);
  int pos = std::lower_bound(block.pairs, block.pairs + block.size_, key_value) - block.pairs;
  for(int i = block.size_ - 1; i >= pos; i--)
  {
    block.pairs[i + 1] = block.pairs[i];
  }
  block.pairs[pos] = key_value;
  block.size_++;
  io_.Update(block, index);
}
#endif