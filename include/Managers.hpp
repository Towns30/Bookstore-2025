#ifndef MANAGERS_HPP
#define MANAGERS_HPP
#include "BlockList.hpp"
#include <array>

struct AccountInfo
{
  std::array<char, 61> user_name_; // 实际长度: 30
  std::array<char, 61> password_; // 实际长度: 30
  int privilege_; // 实际取值范围: 0, 1, 3, 7
};

struct LogInfo
{
  std::array<char, 61> user_id_; // 实际长度：30
  std::array<char, 400> source_;
  int privilege_; // 实际取值范围: 0, 1, 3, 7
  bool result_;
};

struct BookInfo
{
  std::array<char, 61> isbn_; // 实际长度：20
  std::array<char, 61> bookname_; // 实际长度：60
  std::array<char, 61> author_; // 实际长度：60
  std::array<char, 61> keyword_; // 实际长度：60
  long long price_; // 100 * price
  long long enter_price; // 100 * enter_price
  int storage; 
};

struct FinanceInfo
{
  long long income_; // 100 * income
  long long outcome_; // 100 * outcome
};

class LogManager
{
public:
  static LogManager &getInstance()
  {
    static LogManager instance;
    return instance;
  }
  LogManager(const LogManager &) = delete;
  LogManager &operator=(const LogManager &) = delete;

private:
  LogManager();
  ~LogManager();
  BlockList<long long, LogInfo> log_data_;
};

class BookManager
{
public:
  static BookManager &getInstance()
  {
    static BookManager instance;
    return instance;
  }
  BookManager(const BookManager &) = delete;
  BookManager &operator=(const BookManager &) = delete;

private:
  BookManager();
  ~BookManager();
  BlockList<int, BookInfo> book_data_;
  BlockList<std::array<char, 61>, int> isbn_index_;
  BlockList<std::array<char, 61>, int> bookname_index_;
  BlockList<std::array<char, 61>, int> author_index_;
  BlockList<std::array<char, 61>, int> keyword_index_;
};

class AccountManager
{
public:
  static AccountManager &getInstance()
  {
    static AccountManager instance;
    return instance;
  }
  AccountManager(const AccountManager &) = delete;
  AccountManager &operator=(const AccountManager &) = delete;

private:
  AccountManager();
  ~AccountManager();
  BlockList<std::array<char, 61>, AccountInfo> account_data_;
};

class FinanceManager
{
public:
  static FinanceManager &getInstance()
  {
    static FinanceManager instance;
    return instance;
  }
  FinanceManager(const FinanceManager &) = delete;
  FinanceManager &operator=(const FinanceManager &) = delete;

private:
  FinanceManager();
  ~FinanceManager();
  BlockList<long long, FinanceInfo> finance_data_;
};
#endif