#include "../include/Managers.hpp"
#include "../include/Utils.hpp"
#include <array>
#include <cassert>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
void AccountManager::Su(const std::string &user_id, const std::string &passwd)
{
  std::array<char, 61> user_id_array = Utils::StringToArray<61>(user_id);
  auto account_infos = account_data_.Find(user_id_array);
  if (account_infos.empty()) // 没有此user_id
  {
    throw std::runtime_error("Invalid\n");
  }
  assert(account_infos.size() < 2); // 理论上size不可能大于等于2
  auto account_info = account_infos[0];
  std::array<char, 61> passwd_array = Utils::StringToArray<61>(passwd);
  int privilege = account_info.privilege_;
  bool result = false; // 执行结果
  if (passwd == "")    // 未传密码
  {
    if (GetCurrentPrivilege() > privilege)
    {
      result = true;
    }
  }
  else // 传了密码
  {
    if (passwd_array == account_info.passwd_)
    {
      result = true;
    }
  }
  if (!result)
  {
    throw std::runtime_error("Invalid\n");
  }
  user_session_.push_back(
      UserInfo{.user_id_ = user_id, .privilege_ = privilege, .select_book_index_ = -1});
}
void AccountManager::Logout()
{
  if (GetCurrentPrivilege() < 1)
    throw std::runtime_error("Invalid\n"); // 权限检查
  if (user_session_.empty())               // 登录栈为空
  {
    throw std::runtime_error("Invalid\n");
  }
  user_session_.pop_back();
}
void AccountManager::Register(const std::string &user_id, const std::string &passwd,
                              const std::string &user_name)
{
  std::array<char, 61> user_id_array = Utils::StringToArray<61>(user_id);
  auto account_infos = account_data_.Find(user_id_array);
  if (!account_infos.empty()) // 已经有此user_id
  {
    throw std::runtime_error("Invalid\n");
  }
  std::array<char, 61> passwd_array = Utils::StringToArray<61>(passwd);
  std::array<char, 61> user_name_array = Utils::StringToArray<61>(user_name);
  account_data_.Insert(
      user_id_array,
      AccountInfo{.user_name_ = user_name_array, .passwd_ = passwd_array, .privilege_ = 1});
}
void AccountManager::Passwd(const std::string &user_id, const std::string &new_passwd,
                            const std::string &cur_passwd)
{
  if (GetCurrentPrivilege() < 1)
    throw std::runtime_error("Invalid\n"); // 权限检查
  std::array<char, 61> user_id_array = Utils::StringToArray<61>(user_id);
  auto account_infos = account_data_.Find(user_id_array);
  if (account_infos.empty()) // 没有此user_id
  {
    throw std::runtime_error("Invalid\n");
  }
  assert(account_infos.size() < 2); // 理论上size不可能大于等于2
  std::array<char, 61> cur_passwd_array = Utils::StringToArray<61>(cur_passwd);
  int now_privilege = GetCurrentPrivilege();
  bool result = false;  // 执行结果
  if (cur_passwd == "") // 未传密码
  {
    if (GetCurrentPrivilege() == 7)
    {
      result = true;
    }
  }
  else // 传了密码
  {
    if (cur_passwd_array == account_infos[0].passwd_)
    {
      result = true;
    }
  }
  if (!result)
  {
    throw std::runtime_error("Invalid\n");
  }
  std::array<char, 61> new_passwd_array = Utils::StringToArray<61>(new_passwd);
  account_data_.Delete(user_id_array, account_infos[0]);
  account_data_.Insert(user_id_array, AccountInfo{.user_name_ = account_infos[0].user_name_,
                                                  .passwd_ = new_passwd_array,
                                                  .privilege_ = account_infos[0].privilege_});
}
void AccountManager::UserAdd(const std::string &user_id, const std::string &passwd, int privilege,
                             const std::string &user_name)
{
  if (privilege != 1 && privilege != 3)
  {
    throw std::runtime_error("Invalid\n");
  }
  if (GetCurrentPrivilege() < 3)
  {
    throw std::runtime_error("Invalid\n");
  } // 权限检查
  std::array<char, 61> user_id_array = Utils::StringToArray<61>(user_id);
  auto account_infos = account_data_.Find(user_id_array);
  if (!account_infos.empty()) // 已经有此user_id
  {
    throw std::runtime_error("Invalid\n");
  }
  if (privilege >=
      GetCurrentPrivilege()) // 如果待创建帐户的权限等级大于等于当前帐户权限等级则操作失败
  {
    throw std::runtime_error("Invalid\n");
  }
  account_data_.Insert(user_id_array, AccountInfo{.user_name_ = Utils::StringToArray<61>(user_name),
                                                  .passwd_ = Utils::StringToArray<61>(passwd),
                                                  .privilege_ = privilege});
}
void AccountManager::Delete(const std::string &user_id)
{
  if (GetCurrentPrivilege() < 7)
    throw std::runtime_error("Invalid\n"); // 权限检查
  std::array<char, 61> user_id_array = Utils::StringToArray<61>(user_id);
  auto account_infos = account_data_.Find(user_id_array);
  if (account_infos.empty()) // 没有此user_id
  {
    throw std::runtime_error("Invalid\n");
  }
  for (auto user : user_session_)
  {
    if (user.user_id_ == user_id)
    {
      throw std::runtime_error("Invalid\n"); // 正在登录的用户不能删
    }
  }
  assert(account_infos.size() < 2); // 理论上size不可能大于等于2
  account_data_.Delete(user_id_array, account_infos[0]);
}
int AccountManager::GetCurrentPrivilege()
{
  if (user_session_.empty()) // 未登录任何帐户的状态下当前帐户权限视为 {0}
  {
    return 0;
  }
  return user_session_[user_session_.size() - 1].privilege_;
}
int AccountManager::GetCurrentSelectedIndex()
{
  if (user_session_.empty()) // 未登录任何帐户的状态
  {
    return -1;
  }
  return user_session_[user_session_.size() - 1].select_book_index_;
}
void AccountManager::SetCurrentSelectedIndex(const int &index)
{
  if (user_session_.empty()) // 未登录任何帐户的状态
  {
    return;
  }
  user_session_[user_session_.size() - 1].select_book_index_ = index;
}
AccountManager::AccountManager()
{
  account_data_.Initialise("account_data"); // 如果文件未创建会自动创建
  std::array<char, 61> root_array = Utils::StringToArray<61>("root");
  auto root_infos = account_data_.Find(root_array);
  if (root_infos.empty()) // root未创建
  {
    AccountInfo root_info;
    root_info.user_name_ = Utils::StringToArray<61>("root");
    root_info.passwd_ = Utils::StringToArray<61>("sjtu");
    root_info.privilege_ = 7;
    account_data_.Insert(root_array, root_info);
  }
}
void BookManager::Select(const std::string &isbn)
{
  if (AccountManager::getInstance().GetCurrentPrivilege() < 3) // 权限不足
  {
    throw std::runtime_error("Invalid\n");
  }
  if (AccountManager::getInstance().user_session_.empty()) // 无人登录
  {
    throw std::runtime_error("Invalid\n");
  }
  std::array<char, 61> isbn_array = Utils::StringToArray<61>(isbn);
  std::vector<int> now_indexs = isbn_index_.Find(isbn_array);
  assert(now_indexs.size() < 2);
  if (now_indexs.empty()) // 没有这本书, 创建并选中它
  {
    AccountManager::getInstance().SetCurrentSelectedIndex(CreatNewBook(isbn));
  }
  else // 有这本书
  {
    int now_index = now_indexs[0];
    AccountManager::getInstance().SetCurrentSelectedIndex(now_index);
  }
}
void BookManager::Modify(const BookModifyPackage &book_modify_package)
{
  int now_index = AccountManager::getInstance().GetCurrentSelectedIndex();
  if (now_index == -1) // 如未选中图书则操作失败
  {
    throw std::runtime_error("Invalid\n");
  }
  std::vector<BookInfo> book_infos = book_data_.Find(now_index);
  assert(book_infos.size() < 2);
  BookInfo book_info = book_infos[0];
  BookInfo new_book_info = book_info;

  if (book_modify_package.isbn_.has_value())
  {
    std::string new_isbn = book_modify_package.isbn_.value();
    if (new_isbn.empty()) // 附加参数内容为空则操作失败；
    {
      throw std::runtime_error("Invalid\n");
    }
    std::array<char, 61> new_isbn_array = Utils::StringToArray<61>(new_isbn);
    if (new_isbn_array == book_info.isbn_) // 不允许将 ISBN 改为原有的 ISBN
    {
      throw std::runtime_error("Invalid\n");
    }
    if (!isbn_index_.Find(new_isbn_array).empty()) // 本系统中任何两本图书不应有相同 `[ISBN]` 信息
    {
      throw std::runtime_error("Invalid\n");
    }
  }

  if (book_modify_package.author_.has_value())
  {
    std::string new_author = book_modify_package.author_.value();
    if (new_author.empty()) // 附加参数内容为空则操作失败
    {
      throw std::runtime_error("Invalid\n");
    }
  }

  if (book_modify_package.bookname_.has_value())
  {
    std::string new_bookname = book_modify_package.bookname_.value();
    if (new_bookname.empty()) // 附加参数内容为空则操作失败
    {
      throw std::runtime_error("Invalid\n");
    }
  }

  if (book_modify_package.price_.has_value())
  {
    long long new_price = book_modify_package.price_.value();
    if (new_price < 0)
    {
      throw std::runtime_error("Invalid\n");
    }
  }

  if (book_modify_package.keyword_.has_value())
  {
    std::string new_keywords = book_modify_package.keyword_.value();
    std::array<char, 61> new_keywords_array = Utils::StringToArray<61>(new_keywords);
    std::vector<std::array<char, 61>> new_keyword_array_vector = SplitKeywords(new_keywords_array);
    std::set<std::array<char, 61>> check_uniqueness_set;
    for (auto new_keyword : new_keyword_array_vector)
    {
      if (check_uniqueness_set.count(new_keyword))
      {
        throw std::runtime_error("Invalid\n");
      }
      check_uniqueness_set.insert(new_keyword);
    }
  }

  if (book_modify_package.isbn_.has_value())
  {
    std::string new_isbn = book_modify_package.isbn_.value();
    std::array<char, 61> new_isbn_array = Utils::StringToArray<61>(new_isbn);
    isbn_index_.Delete(book_info.isbn_, now_index);
    new_book_info.isbn_ = new_isbn_array;
    isbn_index_.Insert(new_isbn_array, now_index);
  }

  if (book_modify_package.author_.has_value())
  {
    std::string new_author = book_modify_package.author_.value();
    std::array<char, 61> new_author_array = Utils::StringToArray<61>(new_author);
    author_index_.Delete(book_info.author_, now_index);
    new_book_info.author_ = new_author_array;
    author_index_.Insert(new_author_array, now_index);
  }

  if (book_modify_package.bookname_.has_value())
  {
    std::string new_bookname = book_modify_package.bookname_.value();
    std::array<char, 61> new_bookname_array = Utils::StringToArray<61>(new_bookname);
    bookname_index_.Delete(book_info.bookname_, now_index);
    new_book_info.bookname_ = new_bookname_array;
    bookname_index_.Insert(new_bookname_array, now_index);
  }

  if (book_modify_package.price_.has_value())
  {
    long long new_price = book_modify_package.price_.value();
    new_book_info.price_ = new_price;
  }

  if (book_modify_package.keyword_.has_value())
  {
    std::string new_keywords = book_modify_package.keyword_.value();
    std::array<char, 61> new_keywords_array = Utils::StringToArray<61>(new_keywords);
    std::vector<std::array<char, 61>> new_keyword_array_vector = SplitKeywords(new_keywords_array);
    std::set<std::array<char, 61>> check_uniqueness_set;
    std::vector<std::array<char, 61>> cur_keyword_array_vector = SplitKeywords(book_info.keyword_);
    for (auto cur_keyword : cur_keyword_array_vector)
    {
      keyword_index_.Delete(cur_keyword, now_index);
    }
    new_book_info.keyword_ = new_keywords_array;
    for (auto new_keyword : new_keyword_array_vector)
    {
      keyword_index_.Insert(new_keyword, now_index);
    }
  }

  book_data_.Delete(now_index, book_info);
  book_data_.Insert(now_index, new_book_info);
}
void BookManager::Import(const long long &quantity, const long long total_cost)
{
  if (AccountManager::getInstance().GetCurrentPrivilege() < 3) // 权限检查
  {
    throw std::runtime_error("Invalid\n");
  }
  int now_index = AccountManager::getInstance().GetCurrentSelectedIndex();
  if (now_index == -1) // 如未选中图书则操作失败
  {
    throw std::runtime_error("Invalid\n");
  }
  if (quantity <= 0 || total_cost <= 0) // 购入数量为非正整数则操作失败，交易总额为非正数则操作失败
  {
    throw std::runtime_error("Invalid\n");
  }
  std::vector<BookInfo> cur_book_infos = book_data_.Find(now_index);
  assert(cur_book_infos.size() < 2);
  BookInfo cur_book_info = cur_book_infos[0];
  BookInfo new_book_info = cur_book_info;
  new_book_info.storage_ += quantity;
  book_data_.Delete(now_index, cur_book_info);
  book_data_.Insert(now_index, new_book_info);
  FinanceManager::getInstance().AddFinanceReport(0, total_cost);
}
void BookManager::Show(const BookShowPackage &book_show_package)
{
  if (AccountManager::getInstance().GetCurrentPrivilege() < 1) // 权限检查
  {
    throw std::runtime_error("Invalid\n");
  }
  std::vector<int> show_indexs;
  if (book_show_package.type_ == BookShowPackage::ShowType::ALL)
  {
    show_indexs = isbn_index_.GetAll();
  }
  else if (book_show_package.type_ == BookShowPackage::ShowType::ISBN &&
           book_show_package.value_ != "") // 附加参数内容为空则操作失败
  {
    show_indexs = isbn_index_.Find(Utils::StringToArray<61>(book_show_package.value_));
  }
  else if (book_show_package.type_ == BookShowPackage::ShowType::BOOKNAME &&
           book_show_package.value_ != "") // 附加参数内容为空则操作失败
  {
    show_indexs = bookname_index_.Find(Utils::StringToArray<61>(book_show_package.value_));
  }
  else if (book_show_package.type_ == BookShowPackage::ShowType::AUTHOR &&
           book_show_package.value_ != "") // 附加参数内容为空则操作失败
  {
    show_indexs = author_index_.Find(Utils::StringToArray<61>(book_show_package.value_));
  }
  else if (book_show_package.type_ == BookShowPackage::ShowType::KEYWORD &&
           book_show_package.value_ != "") // 附加参数内容为空则操作失败
  {
    show_indexs = keyword_index_.Find(Utils::StringToArray<61>(book_show_package.value_));
  }
  ShowSortedBooks(show_indexs);
}
void BookManager::ShowSortedBooks(std::vector<int> &show_indexs)
{
  if (show_indexs.empty()) // 无满足要求的图书时输出空行
  {
    std::cout << '\n';
    return;
  }
  struct SortISBNIndex
  {
    std::string isbn_;
    int index_;
    bool operator<(const SortISBNIndex &other) const { return isbn_ < other.isbn_; }
  };
  std::vector<SortISBNIndex> sort_list;
  for (auto index : show_indexs)
  {
    std::vector<BookInfo> book_infos = book_data_.Find(index);
    if (!book_infos.empty())
    {
      sort_list.push_back(
          SortISBNIndex{.isbn_ = Utils::ArrayToString<61>(book_infos[0].isbn_), .index_ = index});
    }
  }
  std::sort(sort_list.begin(), sort_list.end());
  for (auto sort_isbn_index : sort_list)
  {
    PrintBook(book_data_.Find(sort_isbn_index.index_)[0]);
  }
}
void BookManager::Buy(const std::string &isbn, const long long &quantity)
{
  if (AccountManager::getInstance().GetCurrentPrivilege() < 1) // 权限检查
  {
    throw std::runtime_error("Invalid\n");
  }
  if (quantity <= 0) // 购买数量为非正整数则操作失败
  {
    throw std::runtime_error("Invalid\n");
  }
  std::array<char, 61> isbn_array = Utils::StringToArray<61>(isbn);
  std::vector<int> indexs = isbn_index_.Find(isbn_array);
  if (indexs.empty())
  {
    throw std::runtime_error("Invalid\n");
  }
  int now_index = indexs[0];
  std::vector<BookInfo> book_infos = book_data_.Find(now_index);
  assert(book_infos.size() < 2);
  BookInfo cur_book_info = book_infos[0];
  BookInfo new_book_info = cur_book_info;
  if (cur_book_info.storage_ < quantity)
  {
    throw std::runtime_error("Invalid\n");
  }
  long long total_price = cur_book_info.price_ * quantity;
  new_book_info.storage_ -= quantity;
  book_data_.Delete(now_index, cur_book_info);
  book_data_.Insert(now_index, new_book_info);
  FinanceManager::getInstance().AddFinanceReport(total_price, 0);
  std::cout << std::fixed << std::setprecision(2) << static_cast<long double>(total_price) / 100.0
            << "\n";
}
int BookManager::CreatNewBook(const std::string &isbn)
{
  std::array<char, 61> isbn_array = Utils::StringToArray<61>(isbn);
  std::vector<int> now_indexs = isbn_index_.Find(isbn_array);
  assert(now_indexs.size() < 2);
  if (!now_indexs.empty()) // 已有则报错
  {
    throw std::runtime_error("Invalid\n");
  }
  total_books_++;
  std::array<char, 61> empty_array = Utils::StringToArray<61>("");
  book_data_.Insert(total_books_, BookInfo{.isbn_ = isbn_array,
                                           .bookname_ = empty_array,
                                           .author_ = empty_array,
                                           .keyword_ = empty_array,
                                           .price_ = 0,
                                           .storage_ = 0});
  isbn_index_.Insert(isbn_array, total_books_);
  return total_books_;
}
void BookManager::PrintBook(BookInfo book_info)
{
  std::string isbn = Utils::ArrayToString<61>(book_info.isbn_);
  std::string bookname = Utils::ArrayToString<61>(book_info.bookname_);
  std::string author = Utils::ArrayToString<61>(book_info.author_);
  std::string keyword = Utils::ArrayToString<61>(book_info.keyword_);
  long double price = book_info.price_ / 100.0;
  int storage = book_info.storage_;
  std::cout << isbn << '\t' << bookname << '\t' << author << '\t' << keyword << '\t' << price
            << '\t' << storage << '\n';
}
void BookManager::ModifyKeywordIndex(std::array<char, 61> cur_keywords_array,
                                     std::array<char, 61> new_keywords_array, int now_index)
{
  std::vector<std::array<char, 61>> cur_keywords_array_vector = SplitKeywords(cur_keywords_array);
  std::vector<std::array<char, 61>> new_keywords_array_vector = SplitKeywords(new_keywords_array);
  std::set<std::string> check_uniqueness_set;
  for (auto new_keyword : new_keywords_array_vector) // `[keyword]` 包含重复信息段则操作失败
  {
    std::string new_keyword_string = Utils::ArrayToString<61>(new_keyword);
    if (check_uniqueness_set.count(new_keyword_string))
    {
      throw std::runtime_error("Invalid\n");
    }
    check_uniqueness_set.insert(new_keyword_string);
  }
  for (auto cur_keyword : cur_keywords_array_vector)
  {
    keyword_index_.Delete(cur_keyword, now_index);
  }
  for (auto new_keyword : new_keywords_array_vector)
  {
    keyword_index_.Insert(new_keyword, now_index);
  }
}
std::vector<std::array<char, 61>> BookManager::SplitKeywords(std::array<char, 61> keywords)
{
  std::vector<std::array<char, 61>> result;
  std::string keywords_string = Utils::ArrayToString<61>(keywords);
  if (keywords_string.empty())
  {
    return result;
  }
  int start = 0;
  int end = keywords_string.find('|');
  while (end != std::string::npos)
  {
    std::string keyword = keywords_string.substr(start, end - start);
    if (!keyword.empty())
    {
      result.push_back(Utils::StringToArray<61>(keyword));
    }
    start = end + 1;
    end = keywords_string.find('|', start);
  }
  std::string last_keyword = keywords_string.substr(start);
  if (!last_keyword.empty())
  {
    result.push_back(Utils::StringToArray<61>(last_keyword));
  }
  return result;
}
BookManager::BookManager()
{
  book_data_.Initialise("book_data");
  isbn_index_.Initialise("isbn_index");
  bookname_index_.Initialise("bookname_index");
  author_index_.Initialise("author_index");
  keyword_index_.Initialise("keyword_index");
  total_books_ = book_data_.CountAllPairs();
}
void FinanceManager::AddFinanceReport(const long long &income, const long long &outcome)
{
  total_count_++;
  finance_data_.Insert(total_count_, FinanceInfo{.income_ = income, .outcome_ = outcome});
}
void FinanceManager::ShowFinanceReport(const long long &count)
{
  std::vector<std::pair<long double, long double>> result;
  if (count > total_count_) // `Count` 大于历史交易总笔数时操作失败
  {
    throw std::runtime_error("Invalid\n");
  }
  if (count == 0) // `Count` 为 0 时输出空行
  {
    std::cout << '\n';
    return;
  }
  long double total_income = 0.0;
  long double total_outcome = 0.0;
  for (int i = total_count_ - count + 1; i <= total_count_; i++)
  {
    std::vector<FinanceInfo> finance_infos = finance_data_.Find(i);
    long double income = finance_infos[0].income_ / 100.0;
    long double outcome = finance_infos[0].outcome_ / 100.0;
    total_income += income;
    total_outcome += outcome;
  }
  std::cout << std::fixed << std::setprecision(2) << "+ " << total_income << " - " << total_outcome
            << '\n';
}
void FinanceManager::ShowAllFinanceReport() // 无 `Count` 参数时，输出所有交易之总额
{
  if (total_count_ == 0)
  {
    std::cout << "+ 0.00 - 0.00\n";
    return;
  }
  ShowFinanceReport(total_count_);
}
FinanceManager::FinanceManager()
{
  finance_data_.Initialise("finance_data");
  total_count_ = finance_data_.CountAllPairs(); // 获取外存中的交易总笔数
}