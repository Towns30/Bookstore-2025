#ifndef BPT_BLOCKLIST_HPP
#define BPT_BLOCKLIST_HPP

#include "MemoryRiver.hpp"
#include <algorithm>
#include <array>
#include <vector>

// using K = std::array<char, 64>;
// using V = int;

template <class K, class V> class BlockList
{
private:
  static constexpr int blockSize = 100;
  struct KeyValue
  {
    K key_;
    V value_;
    friend bool operator==(KeyValue now, KeyValue other)
    {
      if (now.key_ == other.key_)
      {
        if (now.value_ == other.value_)
        {
          return true;
        }
      }
      return false;
    }
    friend bool operator<(KeyValue now, KeyValue other)
    {
      if (now.key_ != other.key_)
      {
        return now.key_ < other.key_;
      }
      if (now.value_ != other.value_)
      {
        return now.value_ < other.value_;
      }
      return false;
    }
    friend bool operator>(KeyValue now, KeyValue other)
    {
      if (now.key_ != other.key_)
      {
        return now.key_ > other.key_;
      }
      if (now.value_ != other.value_)
      {
        return now.value_ > other.value_;
      }
      return false;
    }
    friend bool operator<=(KeyValue now, KeyValue other) { return !(now > other); }
    friend bool operator>=(KeyValue now, KeyValue other) { return !(now < other); }
    friend bool operator!=(KeyValue now, KeyValue other) { return !(now == other); }
  };
  struct Block
  {
    int size_;       // 存储block中有多少元素
    int pre_index_;  // 前缀节点
    int next_index_; // 后缀节点
    KeyValue max_in_block_;
    KeyValue min_in_block_;
    KeyValue pairs_[blockSize]; // 存所有数据
  };
  MemoryRiver<Block, 1> file_writer_;
  // 取出block中的一系列参数
  int GetSize(int index);
  int GetPre(int index);
  int GetNext(int index);
  KeyValue GetMin(int index);
  KeyValue GetMax(int index);
  // 设置block的一系列参数
  void SetSize(int size, int index);
  void SetPre(int pre, int index);
  void SetNext(int next, int index);
  void SetMin(KeyValue min, int index);
  void SetMax(KeyValue max, int index);
  // 取出Block
  Block GetBlock(int index);
  // 放回block
  void SetBlock(Block b, int index);
  void MergeBlock(int index1, int index2);
  void SplitBlock(int index);
  // 创建一个空块，并返回其块索引（0-based）
  int CreateEmptyBlock();
  void InsertPair(int index, KeyValue inserted_pair);
  void DeletePair(int index, KeyValue deleted_pair);
  // 提取出block里键为key的value
  std::vector<V> GetValuesInBlock(int index, K key);

public:
  // 初始化
  void Initialise(std::string file_name);
  // 插入键值对
  void Insert(K key, V value);
  // 删除键值对
  void Delete(K key, V value);
  // 找键与key相同的元素, 没有返回空vector
  std::vector<V> Find(K key);
};
template <class K, class V> void BlockList<K, V>::Initialise(std::string file_name)
{
  file_writer_.Initialise(file_name);
}
template <class K, class V> void BlockList<K, V>::Insert(K key, V value)
{
  int head = file_writer_.GetInfo(1);
  KeyValue inserted_pair = {key, value};
  if (head == -1) // 空链表
  {
    head = CreateEmptyBlock();
    InsertPair(head, inserted_pair);
    file_writer_.WriteInfo(0, 1);
    return;
  }
  int now = head;
  while (GetSize(now) != 0 && GetMax(now) < inserted_pair && GetNext(now) != -1)
  {
    now = GetNext(now);
  }
  InsertPair(now, inserted_pair);
}
template <class K, class V> void BlockList<K, V>::Delete(K key, V value)
{
  int head = file_writer_.GetInfo(1);
  KeyValue deleted_pair = {key, value};
  if (head == -1) // 空链表
  {
    return;
  }
  int now = head;
  while (GetSize(now) != 0 && GetMax(now) < deleted_pair && GetNext(now) != -1)
  {
    now = GetNext(now);
  }
  if (GetSize(now) == 0)
  {
    return;
  }
  DeletePair(now, deleted_pair);
}
template <class K, class V> std::vector<V> BlockList<K, V>::Find(K key)
{
  std::vector<V> values;
  int head = file_writer_.GetInfo(1);
  if (head == -1) // 空链表
  {
    return values;
  }
  int now = head;
  while (GetSize(now) != 0 && GetMax(now).key_ < key && GetNext(now) != -1)
  {
    now = GetNext(now);
  }
  if (GetSize(now) == 0)
  {
    return values;
  }
  while (now != -1)
  {
    if (GetSize(now) == 0)
    {
      now = GetNext(now);
      continue;
    }
    if (GetMin(now).key_ > key)
    {
      break;
    }
    std::vector<V> values_in_pairs = GetValuesInBlock(now, key);
    for (auto value : values_in_pairs)
    {
      values.push_back(value);
    }
    now = GetNext(now);
  }
  return values;
}
template <class K, class V>
void BlockList<K, V>::InsertPair(int index, typename BlockList<K, V>::KeyValue inserted_pair)
{
  Block inserted_block = GetBlock(index);
  int pos = std::lower_bound(inserted_block.pairs_, inserted_block.pairs_ + inserted_block.size_,
                             inserted_pair) -
            inserted_block.pairs_;
  for (int i = inserted_block.size_ - 1; i >= pos; i--)
  {
    inserted_block.pairs_[i + 1] = inserted_block.pairs_[i];
  }
  inserted_block.pairs_[pos] = inserted_pair;
  inserted_block.size_ += 1;
  inserted_block.min_in_block_ = inserted_block.pairs_[0];
  inserted_block.max_in_block_ = inserted_block.pairs_[inserted_block.size_ - 1]; // 更新最大最小
  SetBlock(inserted_block, index);
  if (inserted_block.size_ == blockSize)
  {
    SplitBlock(index);
  }
}
template <class K, class V>
void BlockList<K, V>::DeletePair(int index, typename BlockList<K, V>::KeyValue deleted_pair)
{
  Block deleted_block = GetBlock(index);
  int pos = std::lower_bound(deleted_block.pairs_, deleted_block.pairs_ + deleted_block.size_,
                             deleted_pair) -
            deleted_block.pairs_;
  if (pos == deleted_block.size_ || deleted_block.pairs_[pos] != deleted_pair)
  {
    return;
  }
  for (int i = pos; i <= deleted_block.size_ - 1; i++)
  {
    deleted_block.pairs_[i] = deleted_block.pairs_[i + 1];
  } // 删去此键值对
  deleted_block.size_ -= 1;
  deleted_block.min_in_block_ = deleted_block.pairs_[0];
  deleted_block.max_in_block_ = deleted_block.pairs_[deleted_block.size_ - 1]; // 更新最大最小
  SetBlock(deleted_block, index);
  int pre = GetPre(index);
  int next = GetNext(index);
  if (pre != -1 && GetSize(pre) + GetSize(index) <= blockSize - 1)
  {
    MergeBlock(pre, index);
  }
  else if (next != -1 && GetSize(index) + GetSize(next) <= blockSize - 1)
  {
    MergeBlock(index, next);
  }
}
template <class K, class V> int BlockList<K, V>::GetSize(int index)
{

  return file_writer_.template Read<int>(index, offsetof(Block, size_));
}
template <class K, class V> int BlockList<K, V>::GetPre(int index)
{
  return file_writer_.template Read<int>(index, offsetof(Block, pre_index_));
}
template <class K, class V> int BlockList<K, V>::GetNext(int index)
{
  return file_writer_.template Read<int>(index, offsetof(Block, next_index_));
}
template <class K, class V> typename BlockList<K, V>::KeyValue BlockList<K, V>::GetMin(int index)
{
  return file_writer_.template Read<typename BlockList<K, V>::KeyValue>(
      index, offsetof(Block, min_in_block_));
}
template <class K, class V> typename BlockList<K, V>::KeyValue BlockList<K, V>::GetMax(int index)
{
  return file_writer_.template Read<typename BlockList<K, V>::KeyValue>(
      index, offsetof(Block, max_in_block_));
}
template <class K, class V> void BlockList<K, V>::SetSize(int size, int index)
{
  file_writer_.Update(size, index, offsetof(Block, size_));
}
template <class K, class V> void BlockList<K, V>::SetPre(int pre, int index)
{
  file_writer_.Update(pre, index, offsetof(Block, pre_index_));
}
template <class K, class V> void BlockList<K, V>::SetNext(int next, int index)
{
  file_writer_.Update(next, index, offsetof(Block, next_index_));
}
template <class K, class V>
void BlockList<K, V>::SetMin(typename BlockList<K, V>::KeyValue min, int index)
{
  file_writer_.Update(min, index, offsetof(Block, min_in_block_));
}
template <class K, class V>
void BlockList<K, V>::SetMax(typename BlockList<K, V>::KeyValue max, int index)
{
  file_writer_.Update(max, index, offsetof(Block, max_in_block_));
}
template <class K, class V> typename BlockList<K, V>::Block BlockList<K, V>::GetBlock(int index)
{
  Block got_block;
  got_block = file_writer_.ReadBlock(index);
  return got_block;
}
template <class K, class V>
void BlockList<K, V>::SetBlock(typename BlockList<K, V>::Block set_block, int index)
{
  file_writer_.UpdateBlock(set_block, index);
}
template <class K, class V> int BlockList<K, V>::CreateEmptyBlock()
{
  int pos;
  int head = file_writer_.GetInfo(1);
  Block empty_block{};
  empty_block.next_index_ = -1; // 把next设为-1
  empty_block.pre_index_ = -1;  // 把前缀节点设为-1
  pos = file_writer_.Write(empty_block);
  if(head == -1)
  {
    file_writer_.WriteInfo(pos, 1);
  }
  return pos;
}
template <class K, class V> void BlockList<K, V>::MergeBlock(int index1, int index2)
{
  int size1 = GetSize(index1);
  int size2 = GetSize(index2);
  Block block1 = GetBlock(index1);
  Block block2 = GetBlock(index2);
  int next = GetNext(index2);
  for (int i = block1.size_; i <= block1.size_ + block2.size_ - 1; i++)
  {
    block1.pairs_[i] = block2.pairs_[i - block1.size_];
  }
  block1.size_ = size1 + size2; // 更新size
  block1.min_in_block_ = block1.pairs_[0];
  block1.max_in_block_ = block1.pairs_[block1.size_ - 1]; // 更新最大最小
  block1.next_index_ = next;                              // 更新next
  if (next != -1)
  {
    SetPre(index1, next);
  }
  SetBlock(block1, index1);
  file_writer_.Delete(index2); // 将第二块标记为空闲
}
template <class K, class V> void BlockList<K, V>::SplitBlock(int index)
{
  int size1 = blockSize / 2, size2 = blockSize - size1;
  int next = CreateEmptyBlock();
  Block now_block = GetBlock(index);
  int next_next = now_block.next_index_;
  Block new_block = GetBlock(next);
  for (int i = size1; i <= size1 + size2 - 1; i++)
  {
    new_block.pairs_[i - size1] = now_block.pairs_[i];
    now_block.pairs_[i] = {};
  } // 分配pairs
  now_block.size_ = size1;
  new_block.size_ = size2;      // 更新size
  now_block.next_index_ = next; // 更新第一个块的next
  new_block.pre_index_ = index; // 更新第二个块的pre
  new_block.next_index_ = next_next; // 更新第二个块的next
  if (next_next != -1)
  {
    SetPre(next, next_next); // 设置第二个块后一个块的pre
  }
  now_block.min_in_block_ = now_block.pairs_[0];
  now_block.max_in_block_ = now_block.pairs_[now_block.size_ - 1];
  new_block.min_in_block_ = new_block.pairs_[0];
  new_block.max_in_block_ = new_block.pairs_[new_block.size_ - 1]; // 更新最大最小
  SetBlock(now_block, index);
  SetBlock(new_block, next);
}
template <class K, class V> std::vector<V> BlockList<K, V>::GetValuesInBlock(int index, K key)
{
  std::vector<V> values;
  Block now_block = GetBlock(index);
  for (int i = 0; i <= now_block.size_ - 1; i++)
  {
    if (now_block.pairs_[i].key_ == key)
    {
      values.push_back(now_block.pairs_[i].value_);
    }
  }
  return values;
}
#endif