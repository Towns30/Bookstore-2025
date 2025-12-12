#include "../include/BlockList.hpp"
#include <array>
#include <iostream>
#include <string>
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