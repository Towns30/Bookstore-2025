#ifndef UTILS_HPP
#define UTILS_HPP
#include <array>
#include <stdexcept>
#include <string>

namespace Utils
{
template <int N> std::array<char, N> StringToArray(const std::string &a)
{
  std::array<char, N> array_in;
  for (int j = 0; j <= N - 1; j++)
  {
    if (j < a.size())
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
template <int N> std::string ArrayToString(const std::array<char, N> &a)
{
  return std::string(a.data());
}
inline long long Pow(long long x, long long y)
{
  long long result = 1;
  while (y > 0)
  {
    result *= x;
    y--;
  }
  return result;
}
inline int StringToInt(std::string a) // a已保证合法整数
{
  int result = 0;
  for(auto c : a)
  {
    result = result * 10 + (c - '0');
  }
  return result;
}
inline void CheckISBN(const std::string &isbn)
{
  if (isbn.size() > 20)
  {
    throw std::runtime_error("Invalid\n");
  }
  if (isbn.empty())
  {
    throw std::runtime_error("Invalid\n");
  }
  for (auto c : isbn)
  {
    if (int(c) <= 32 || int(c) > 126)
    {
      throw std::runtime_error("Invalid\n");
    }
  }
}
inline void CheckBookName(const std::string &bookname)
{
  if (bookname.size() > 60)
  {
    throw std::runtime_error("Invalid\n");
  }
  if (bookname.empty())
  {
    throw std::runtime_error("Invalid\n");
  }
  for (auto c : bookname)
  {
    if (int(c) <= 32 || int(c) > 126 || c == '\"')
    {
      throw std::runtime_error("Invalid\n");
    }
  }
}
inline void CheckAuthor(const std::string &author)
{
  if (author.size() > 60)
  {
    throw std::runtime_error("Invalid\n");
  }
  if (author.empty())
  {
    throw std::runtime_error("Invalid\n");
  }
  for (auto c : author)
  {
    if (int(c) <= 32 || int(c) > 126 || c == '\"')
    {
      throw std::runtime_error("Invalid\n");
    }
  }
}
inline void CheckUserID(const std::string &user_id)
{
  if (user_id.size() > 30)
  {
    throw std::runtime_error("Invalid\n");
  }
  if (user_id.empty())
  {
    throw std::runtime_error("Invalid\n");
  }
  for (auto c : user_id)
  {
    if ((c < '0' || c > '9') && (c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && c != '_')
    {
      throw std::runtime_error("Invalid\n");
    }
  }
}
inline void CheckPasswd(const std::string &passwd)
{
  if (passwd.size() > 30)
  {
    throw std::runtime_error("Invalid\n");
  }
  if (passwd.empty())
  {
    throw std::runtime_error("Invalid\n");
  }
  for (auto c : passwd)
  {
    if ((c < '0' || c > '9') && (c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && c != '_')
    {
      throw std::runtime_error("Invalid\n");
    }
  }
}
inline void CheckUserName(const std::string &user_name)
{
  if (user_name.size() > 30)
  {
    throw std::runtime_error("Invalid\n");
  }
  if (user_name.empty())
  {
    throw std::runtime_error("Invalid\n");
  }
  for (auto c : user_name)
  {
    if (int(c) <= 32 || int(c) > 126)
    {
      throw std::runtime_error("Invalid\n");
    }
  }
}
inline void CheckPrivilege(const std::string &privilege)
{
  if (privilege != "0" && privilege != "1" && privilege != "3" && privilege != "7")
  {
    throw std::runtime_error("Invalid\n");
  }
}
inline void CheckKeyword(const std::string &keyword)
{
  if (keyword.size() > 60)
  {
    throw std::runtime_error("Invalid\n");
  }
  if (keyword.empty())
  {
    throw std::runtime_error("Invalid\n");
  }
  for (auto c : keyword)
  {
    if (int(c) <= 32 || int(c) > 126 || c == '\"' || c == '|')
    {
      throw std::runtime_error("Invalid\n");
    }
  }
}
inline void CheckKeywords(const std::string &keywords) // "||"这种情况会在manager上被判错
{
  if (keywords[0] == '|' || keywords[keywords.size() - 1] == '|')
  {
    throw std::runtime_error("Invalid\n");
  }
  if (keywords.size() > 60)
  {
    throw std::runtime_error("Invalid\n");
  }
  if (keywords.empty())
  {
    throw std::runtime_error("Invalid\n");
  }
  for (auto c : keywords)
  {
    if (int(c) <= 32 || int(c) > 126 || c == '\"')
    {
      throw std::runtime_error("Invalid\n");
    }
  }
}
inline void CheckQuantity(const std::string &quantity)
{
  if (quantity.size() > 10)
  {
    throw std::runtime_error("Invalid\n");
  }
  if (quantity.empty())
  {
    throw std::runtime_error("Invalid\n");
  }
  if (quantity[0] == '0' && quantity.size() > 1) // 前导0
  {
    throw std::runtime_error("Invalid\n");
  }
  for (auto c : quantity)
  {
    if (c < '0' || c > '9')
    {
      throw std::runtime_error("Invalid\n");
    }
  }
}
inline long long CheckPrice(const std::string &price) // 返回price * 100
{
  if (price.size() > 13)
  {
    throw std::runtime_error("Invalid\n");
  }
  if (price.empty())
  {
    throw std::runtime_error("Invalid\n");
  }
  int count_dot = 0;
  for (char c : price)
  {
    if (c == '.')
    {
      count_dot++;
    }
    else if (c < '0' || c > '9')
    {
      throw std::runtime_error("Invalid\n");
    }
  }
  if (count_dot > 1)
  {
    throw std::runtime_error("Invalid\n");
  }
  if ((price[0] == '0' && price.size() > 1 && price[1] != '.') || price[0] == '.' ||
      price[price.size() - 1] == '.') // 前导零，'.'开头，'.'结尾
  {
    throw std::runtime_error("Invalid\n");
  }
  int dot_pos = price.size();
  for (int i = 0; i <= price.size() - 1; i++)
  {
    if (price[i] == '.')
    {
      dot_pos = i;
    }
  }
  if (dot_pos != price.size() && price.size() - 1 - dot_pos > 2)
  {
    throw std::runtime_error("Invalid\n");
  }
  long long result_integer = 0;
  long long result_decimal = 0;
  long long result = 0;
  for (char c : price)
  {
    if (c != '.')
    {
      result_integer = result_integer * 10 + (long long)(c - '0');
    }
    else
    {
      break;
    }
  }
  if (dot_pos != price.size())
  {
    for (int i = dot_pos + 1; i <= price.size() - 1; i++)
    {
      result_decimal = result_decimal * 10 + (long long)(price[i] - '0');
    }
  }
  if (dot_pos != price.size())
  {
    result = result_integer * 100 + result_decimal * Pow(10, 3 - price.size() + dot_pos);
  }
  else
  {
    result = result_integer * 100;
  }
  return result;
}
inline long long CheckTotalCost(const std::string &total_cost)
{
  if (total_cost.size() > 13)
  {
    throw std::runtime_error("Invalid\n");
  }
  if (total_cost.empty())
  {
    throw std::runtime_error("Invalid\n");
  }
  int count_dot = 0;
  for (char c : total_cost)
  {
    if (c == '.')
    {
      count_dot++;
    }
    else if (c < '0' || c > '9')
    {
      throw std::runtime_error("Invalid\n");
    }
  }
  if (count_dot > 1)
  {
    throw std::runtime_error("Invalid\n");
  }
  if ((total_cost[0] == '0' && total_cost.size() > 1 && total_cost[1] != '.') ||
      total_cost[0] == '.' || total_cost[total_cost.size() - 1] == '.') // 前导零，'.'开头，'.'结尾
  {
    throw std::runtime_error("Invalid\n");
  }
  int dot_pos = total_cost.size();
  for (int i = 0; i <= total_cost.size() - 1; i++)
  {
    if (total_cost[i] == '.')
    {
      dot_pos = i;
    }
  }
  if (dot_pos != total_cost.size() && total_cost.size() - 1 - dot_pos > 2)
  {
    throw std::runtime_error("Invalid\n");
  }
  long long result_integer = 0;
  long long result_decimal = 0;
  long long result = 0;
  for (char c : total_cost)
  {
    if (c != '.')
    {
      result_integer = result_integer * 10 + (long long)(c - '0');
    }
    else
    {
      break;
    }
  }
  if (dot_pos != total_cost.size())
  {
    for (int i = dot_pos + 1; i <= total_cost.size() - 1; i++)
    {
      result_decimal = result_decimal * 10 + (long long)(total_cost[i] - '0');
    }
  }
  if (dot_pos != total_cost.size())
  {
    result = result_integer * 100 + result_decimal * Pow(10, 3 - total_cost.size() + dot_pos);
  }
  else
  {
    result = result_integer * 100;
  }
  return result;
}
inline void CheckCount(const std::string &count)
{
  if (count.size() > 10)
  {
    throw std::runtime_error("Invalid\n");
  }
  if (count.empty())
  {
    throw std::runtime_error("Invalid\n");
  }
  if (count[0] == '0' && count.size() > 1) // 前导0
  {
    throw std::runtime_error("Invalid\n");
  }
  for (auto c : count)
  {
    if (c < '0' || c > '9')
    {
      throw std::runtime_error("Invalid\n");
    }
  }
}

} // namespace Utils
#endif