#ifndef MANAGERS_HPP
#define MANAGERS_HPP
#include "BlockList.hpp"
#include <array>
#include <optional>
#include <string>

struct UserInfo
{
  std::string user_id_;
  int privilege_;
  int select_book_index_;
};

struct AccountInfo
{
  std::array<char, 61> user_name_; // 实际长度: 30
  std::array<char, 61> passwd_;    // 实际长度: 30
  int privilege_;                  // 实际取值范围: 0, 1, 3, 7
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
  std::array<char, 61> isbn_;     // 实际长度：20
  std::array<char, 61> bookname_; // 实际长度：60
  std::array<char, 61> author_;   // 实际长度：60
  std::array<char, 61> keyword_;  // 实际长度：60
  long long price_;               // 100 * price
  long long storage_;
};

struct FinanceInfo
{
  long long income_;  // 100 * income
  long long outcome_; // 100 * outcome
};
struct BookModifyPackage
{
  std::optional<std::string> isbn_;
  std::optional<std::string> bookname_;
  std::optional<std::string> author_;
  std::optional<std::string> keyword_;
  std::optional<long long> price_; // 存储 price * 100
};
struct BookShowPackage
{
  enum ShowType
  {
    ALL,
    ISBN,
    BOOKNAME,
    AUTHOR,
    KEYWORD
  } type_ = ALL;
  std::string value_ = "";
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

class BookManager // index均为1_based
{
public:
  static BookManager &getInstance()
  {
    static BookManager instance;
    return instance;
  }
  BookManager(const BookManager &) = delete;
  BookManager &operator=(const BookManager &) = delete;
  void Select(const std::string &isbn);
  void Modify(const BookModifyPackage &book_modify_package);
  void Import(const long long &quantity, const long long total_cost);
  void Show(const BookShowPackage &book_show_package);
  void Buy(const std::string &isbn, const long long &quantity);
  BookManager();
  ~BookManager();

private:
  void ShowSortedBooks(std::vector<int> &indexs);
  int CreatNewBook(const std::string &isbn); // 返回新建的index, isbn重复则报错
  void PrintBook(BookInfo book_info);
  void ModifyKeywordIndex(std::array<char, 61> cur_keywords_array,
                          std::array<char, 61> new_keywords_array, int now_index);
  std::vector<std::array<char, 61>> SplitKeywords(std::array<char, 61> keywords);
  BlockList<int, BookInfo> book_data_;
  BlockList<std::array<char, 61>, int> isbn_index_;
  BlockList<std::array<char, 61>, int> bookname_index_;
  BlockList<std::array<char, 61>, int> author_index_;
  BlockList<std::array<char, 61>, int> keyword_index_;
  int total_books_;
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
  void Su(const std::string &user_id, const std::string &passwd = "");
  void Logout();
  void Register(const std::string &user_id, const std::string &passwd,
                const std::string &user_name);
  void Passwd(const std::string &user_id, const std::string &new_passwd,
              const std::string &cur_passwd = "");
  void UserAdd(const std::string &user_id, const std::string &passwd, int privilege,
               const std::string &name);
  void Delete(const std::string &user_id);
  std::vector<UserInfo> user_session_;
  int GetCurrentPrivilege();
  int GetCurrentSelectedIndex();
  void SetCurrentSelectedIndex(const int &index);
  AccountManager();
  ~AccountManager();

private:
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
  void AddFinanceReport(const long long &income, const long long &outcome);
  void ShowFinanceReport(const long long &count);
  void ShowAllFinanceReport();
  FinanceManager();
  ~FinanceManager();

private:
  BlockList<long long, FinanceInfo> finance_data_;
  long long total_count_;
};
#endif