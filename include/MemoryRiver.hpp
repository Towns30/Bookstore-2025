#ifndef BPT_MEMORYRIVER_HPP
#define BPT_MEMORYRIVER_HPP

#include <fstream>
#include <iostream>

using std::fstream;
using std::ifstream;
using std::ofstream;
using std::string;

template <typename T, int info_len = 2> class MemoryRiver
{
private:
  fstream file_;
  string file_name_;
  int sizeofT_ = sizeof(T);

public:
  MemoryRiver() = default;

  ~MemoryRiver() { file_.close(); }

  MemoryRiver(const string &file_name_) : file_name_(file_name_) {}

  void Initialise(string FN = "")
  {
    if (FN != "")
    {
      file_name_ = FN;
    }
    file_.open(file_name_, std::ios::in | std::ios::out | std::ios::binary);
    if (!file_.is_open())
    {
      file_.clear();
      file_.open(file_name_, std::ios::out | std::ios::binary);
      int tmp = 0;
      for (int i = 0; i < info_len; ++i)
      {
        file_.write(reinterpret_cast<char *>(&tmp), sizeof(int));
      }
    }
    file_.close();
    file_.open(file_name_, std::ios::out | std::ios::in | std::ios::binary);
  }

  // 读出第n个int的值赋给tmp，1_base
  void GetInfo(int &tmp, int n)
  {
    if (n > info_len)
    {
      return;
    }
    file_.seekg((n - 1) * sizeof(int));
    file_.read(reinterpret_cast<char *>(&tmp), sizeof(int));
  }

  // 将tmp写入第n个int的位置，1_base
  void WriteInfo(int tmp, int n)
  {
    if (n > info_len)
    {
      return;
    }
    file_.seekp((n - 1) * sizeof(int));
    file_.write(reinterpret_cast<const char *>(&tmp), sizeof(int));
  }

  // 在文件合适位置写入类对象t，并返回写入的位置索引index
  // 位置索引意味着当输入正确的位置索引index，在以下三个函数中都能顺利的找到目标对象进行操作
  // 位置索引index可以取为对象写入的起始位置
  int Write(T &t)
  {
    int count;
    GetInfo(count, 1);
    // std::cerr << "count: " << count << std::endl;
    file_.seekp(info_len * sizeof(int) + count * sizeofT_);
    file_.write(reinterpret_cast<char *>(&t), sizeofT_);
    count++;
    WriteInfo(count, 1);
    return count;
  }

  // 用t的值更新位置索引index对应的对象，保证调用的index(1-based)都是由write函数产生
  void Update(T &t, const int index)
  {
    file_.seekp(info_len * sizeof(int) + (index - 1) * sizeofT_);
    file_.write(reinterpret_cast<char *>(&t), sizeofT_);
  }

  // 读出位置索引index对应的T对象的值并赋值给t，保证调用的index都是由write函数产生
  void Read(T &t, const int index)
  {
    file_.seekg(info_len * sizeof(int) + (index - 1) * sizeofT_);
    file_.read(reinterpret_cast<char *>(&t), sizeofT_);
  }
  template <typename P> void Peek(P &pair, const int index, const int &delta) // delte:偏移量
  {
    file_.seekg(info_len * sizeof(int) + (index - 1) * sizeofT_ + delta);
    file_.read(reinterpret_cast<char *>(&pair), sizeof(P));
  }
  // 删除位置索引index对应的对象(不涉及空间回收时，可忽略此函数)，保证调用的index都是由write函数产生
};

#endif // BPT_MEMORYRIVER_HPP