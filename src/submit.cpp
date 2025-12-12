#include <algorithm>
#include <array>
#include <vector>
#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>

using std::fstream;
using std::ifstream;
using std::ofstream;
using std::string;

// using K = std::array<char, 64>;
// using V = int;
// const int blockSize = 256;
// struct Block
// {
//   int size_;
//   int pre_index_;
//   int next_index_;
//   int max_in_block_;
//   int min_in_block_;
//   KeyValue pairs[blockSize];
// };
// struct KeyValue
// {
//   K key_;
//   V value_;
// };
// using B = Block;
// const int info_len_ = 2;
template <class B, int info_len_ = 2> class MemoryRiver
{
private:
  fstream file_;
  string file_name_;
  // free_head_和count_均存于文件的最开头，不属于对外开放的info
  int free_head_; // 存储空块链表的头指针
  int count_; // 记录当前状态下包含空块一共有多少块，相当于记录当前开了多少外存空间
  int size_of_B_ = sizeof(B);
  const int interOffset = 2 * sizeof(int); // 为free_head_和count_预留的外存空间

public:
  MemoryRiver() = default;
  // 构造函数
  MemoryRiver(const string &file_name) : file_name_(file_name) {}
  // 析构函数，关闭文件
  ~MemoryRiver()
  {
    file_.seekp(0);
    file_.write(reinterpret_cast<char *>(&free_head_), sizeof(int));
    file_.write(reinterpret_cast<char *>(&count_), sizeof(int));
    file_.close();
  }
  // 初始化文件打开方式，创建文件，并打开
  void Initialise(string FN = "");
  // 取第n个info(n为1—based)
  int GetInfo(const int n);
  // 将tmp写入第n(1—based)个info位置
  void WriteInfo(const int tmp, const int n);
  // 往空块链表头指针位置写入一个Block, 并返回其位置index，index为以块为单位的块索引(0_based)
  int Write(const B &b);
  // 更新某个Block的某个pair,
  // offset为pair位置相对于这个块的偏移值，index为以块为单位的块索引(0_based)
  template <class T> void Update(const T &t, const int index, const int offset);
  // 更新某个Block，index为以块为单位的块索引(0_based)
  void UpdateBlock(const B &b, const int index);
  // 读一个Block, index为以块为单位的块索引(0_based)
  B ReadBlock(const int index);
  // 读某个Block的某个信息, offset为信息位置相对于这个块的偏移值，index为以块为单位的块索引(0_based)
  template <class T> T Read(const int index, const int offset);
  // 将一个block标记为空闲, 通过将其设为空闲链表的头节点
  void Delete(const int index);
  // 返回下一个空块的地址（空块只有一个数据，就是存在块最前面四个字节的next），index为以块为单位的块索引(0_based)
  int GetNextFree(const int index);
  // 在本块写入下一个空块的地址（空块只有一个数据，就是存在块最前面四个字节的next），index为以块为单位的块索引(0_based)
  void WriteNextFree(const int index, int next);
};
template <class B, int info_len_> void MemoryRiver<B, info_len_>::Initialise(string FN)
{
  if (FN != "")
  {
    file_name_ = FN;
  }
  if (!std::filesystem::exists(file_name_))
  {
    count_ = 0;
    free_head_ = -1; // 开始无块
    file_.open(file_name_, std::ios::out);
    int tmp1 = 0, tmp2 = -1;
    file_.write(reinterpret_cast<char *>(&tmp2), sizeof(int));
    file_.write(reinterpret_cast<char *>(&tmp1), sizeof(int));
    file_.write(reinterpret_cast<char *>(&tmp2), sizeof(int));
    file_.close();
    file_.open(file_name_, std::ios::out | std::ios::binary | std::ios::in);
  }
  else
  {
    file_.open(file_name_, std::ios::out | std::ios::binary | std::ios::in);
    file_.seekg(0);
    file_.read(reinterpret_cast<char *>(&free_head_), sizeof(int));
    file_.read(reinterpret_cast<char *>(&count_), sizeof(int));
  }
}
template <class B, int info_len_> int MemoryRiver<B, info_len_>::GetInfo(int n)
{
  int result;
  if (n > info_len_)
    return 0;
  file_.seekg(interOffset + (n - 1) * size_of_B_);
  file_.read(reinterpret_cast<char *>(&result), sizeof(int));
  return result;
}
template <class B, int info_len_> void MemoryRiver<B, info_len_>::WriteInfo(const int tmp, int n)
{
  if (n > info_len_)
    return;
  file_.seekp(interOffset + (n - 1) * size_of_B_);
  file_.write(reinterpret_cast<const char *>(&tmp), sizeof(int));
}
template <class B, int info_len_> int MemoryRiver<B, info_len_>::Write(const B &b)
{
  if (free_head_ == -1) // 无空块
  {
    file_.seekp(interOffset + info_len_ * sizeof(int) + count_ * size_of_B_);
    file_.write(reinterpret_cast<const char *>(&b), size_of_B_);
    return (count_++);
  }
  int pos = free_head_;
  file_.seekp(interOffset + info_len_ * sizeof(int) + free_head_ * size_of_B_);
  free_head_ = GetNextFree(free_head_);
  file_.write(reinterpret_cast<const char *>(&b), size_of_B_);
  return pos;
}
template <class B, int info_len_>
template <class T>
void MemoryRiver<B, info_len_>::Update(const T &t, const int index, const int offset)
{
  file_.seekp(interOffset + info_len_ * sizeof(int) + index * size_of_B_ + offset);
  file_.write(reinterpret_cast<const char *>(&t), sizeof(T));
}
template <class B, int info_len_> B MemoryRiver<B, info_len_>::ReadBlock(const int index)
{
  B b;
  file_.seekg(interOffset + info_len_ * sizeof(int) + index * size_of_B_);
  file_.read(reinterpret_cast<char *>(&b), size_of_B_);
  return b;
}
template <class B, int info_len_>
template <class T>
T MemoryRiver<B, info_len_>::Read(const int index, const int offset)
{
  T t;
  file_.seekg(interOffset + info_len_ * sizeof(int) + index * size_of_B_ + offset);
  file_.read(reinterpret_cast<char *>(&t), sizeof(T));
  return t;
}
template <class B, int info_len_> void MemoryRiver<B, info_len_>::Delete(const int index)
{
  WriteNextFree(index, free_head_);
  free_head_ = index;
}
template <class B, int info_len_> int MemoryRiver<B, info_len_>::GetNextFree(const int index)
{
  int next;
  file_.seekg(interOffset + info_len_ * sizeof(int) + index * size_of_B_);
  file_.read(reinterpret_cast<char *>(&next), sizeof(int));
  return next;
}
template <class B, int info_len_>
void MemoryRiver<B, info_len_>::WriteNextFree(const int index, int next)
{
  file_.seekp(interOffset + info_len_ * sizeof(int) + index * size_of_B_);
  file_.write(reinterpret_cast<char *>(&next), sizeof(int));
}
template <class B, int info_len_>
void MemoryRiver<B, info_len_>::UpdateBlock(const B &b, const int index)
{
  file_.seekp(interOffset + info_len_ * sizeof(int) + index * size_of_B_);
  file_.write(reinterpret_cast<const char *>(&b), size_of_B_);
}

// using K = std::array<char, 64>;
// using V = int;

template <class K, class V> class BlockList
{
private:
  static constexpr int blockSize = 400;
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
    if (GetMin(now).key_ > key)
    {
      break;
    }
    if (GetSize(now) == 0)
    {
      continue;
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
  if (deleted_block.pairs_[pos] != deleted_pair)
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
    file_writer_.WriteInfo(0, 1);
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
  int size1 = blockSize / 2, size2 = blockSize / 2;
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

std::array<char, 64> StringToArray(std::string a)
{
  std::array<char, 64> array_in;
  for (int j = 0; j <= 63; j++)
  {
    if (j <= a.size() - 1)
    {
      array_in[j] = a[j];
    }
    else
    {
      array_in[j] = 0;
    }
  }
  return array_in;
}
int main()
{
  BlockList<std::array<char, 64>, int> block_list;
  block_list.Initialise("data");
  int n;
  std::cin >> n;
  for (int i = 0; i <= n - 1; i++)
  {
    std::string operation;
    std::cin >> operation;
    if (operation == "insert")
    {
      std::string key;
      int value;
      std::cin >> key >> value;
      std::array<char, 64> array_in = StringToArray(key);
      block_list.Insert(array_in, value);
    }
    else if (operation == "delete")
    {
      std::string key;
      int value;
      std::cin >> key >> value;
      std::array<char, 64> array_in = StringToArray(key);
      block_list.Delete(array_in, value);
    }
    else if (operation == "find")
    {
      std::string key;
      std::cin >> key;
      std::array<char, 64> array_in = StringToArray(key);
      std::vector<int> values = block_list.Find(array_in);
      if (values.empty())
      {
        std::cout << "null";
      }
      else
      {
        for (auto value : values)
        {
          std::cout << value << ' ';
        }
      }
      std::cout << "\n";
    }
  }
}
