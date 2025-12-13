#ifndef BPT_MEMORYRIVER_HPP
#define BPT_MEMORYRIVER_HPP

#include <array>
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
    // FIXME(towns) add support for custom info_len
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
  file_.seekg(interOffset + (n - 1) * sizeof(int));
  file_.read(reinterpret_cast<char *>(&result), sizeof(int));
  return result;
}
template <class B, int info_len_> void MemoryRiver<B, info_len_>::WriteInfo(const int tmp, int n)
{
  if (n > info_len_)
    return;
  file_.seekp(interOffset + (n - 1) * sizeof(int));
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
  free_head_ = GetNextFree(free_head_);
  file_.seekp(interOffset + info_len_ * sizeof(int) + pos * size_of_B_);
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
#endif // BPT_MEMORYRIVER_HPP