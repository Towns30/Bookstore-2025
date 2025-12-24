#include "../include/Statement.hpp"
#include "../include/Managers.hpp"


void SuStatement::Execute() { AccountManager::getInstance().Su(user_id_, passwd_); }
void LogoutStatement::Execute()
{
  if (AccountManager::getInstance().GetCurrentPrivilege() < 1) // 权限检查
  {
    throw std::runtime_error("Invalid\n");
  }
  AccountManager::getInstance().Logout();
}
void RegisterStatement::Execute()
{
  AccountManager::getInstance().Register(user_id_, passwd_, user_name_);
}
void PasswdStatement::Execute()
{
  if (AccountManager::getInstance().GetCurrentPrivilege() < 1) // 权限检查
  {
    throw std::runtime_error("Invalid\n");
  }
  AccountManager::getInstance().Passwd(user_id_, new_passwd_, cur_passwd_);
}
void UserAddStatement::Execute()
{
  if (AccountManager::getInstance().GetCurrentPrivilege() < 3) // 权限检查
  {
    throw std::runtime_error("Invalid\n");
  }
  AccountManager::getInstance().UserAdd(user_id_, passwd_, privilege_, user_name_);
}
void DeleteStatement::Execute()
{
  if (AccountManager::getInstance().GetCurrentPrivilege() < 7) // 权限检查
  {
    throw std::runtime_error("Invalid\n");
  }
  AccountManager::getInstance().Delete(user_id_);
}
void ShowBookStatement::Execute()
{
  if (AccountManager::getInstance().GetCurrentPrivilege() < 1) // 权限检查
  {
    throw std::runtime_error("Invalid\n");
  }
  BookShowPackage book_show_package;
  if(all_show_)
  {
    book_show_package.type_ = BookShowPackage::ShowType::ALL;
  }
  else if(!isbn_.empty())
  {
    book_show_package.type_ = BookShowPackage::ShowType::ISBN;
    book_show_package.value_ = isbn_;
  }
  else if(!bookname_.empty())
  {
    book_show_package.type_ = BookShowPackage::ShowType::BOOKNAME;
    book_show_package.value_ = bookname_;
  }
  else if(!author_.empty())
  {
    book_show_package.type_ = BookShowPackage::ShowType::AUTHOR;
    book_show_package.value_ = author_;
  }
  else if(!keyword_.empty())
  {
    book_show_package.type_ = BookShowPackage::ShowType::KEYWORD;
    book_show_package.value_ = keyword_;
  }
  else
  {
    throw std::runtime_error("Invalid\n");
  }
  BookManager::getInstance().Show(book_show_package);
}
void BuyStatement::Execute()
{
  if (AccountManager::getInstance().GetCurrentPrivilege() < 1) // 权限检查
  {
    throw std::runtime_error("Invalid\n");
  }
  BookManager::getInstance().Buy(isbn_, quantity_);
}
void SelectStatement::Execute()
{
  if (AccountManager::getInstance().GetCurrentPrivilege() < 3) // 权限检查
  {
    throw std::runtime_error("Invalid\n");
  }
  BookManager::getInstance().Select(isbn_);
}
void ModifyStatement::Execute()
{
  if (AccountManager::getInstance().GetCurrentPrivilege() < 3) // 权限检查
  {
    throw std::runtime_error("Invalid\n");
  }
  BookModifyPackage book_modify_package;
  if (!this->isbn_.empty())
  {
    book_modify_package.isbn_ = this->isbn_;
  }
  if (!this->bookname_.empty())
  {
    book_modify_package.bookname_ = this->bookname_;
  }
  if (!this->author_.empty())
  {
    book_modify_package.author_ = this->author_;
  }
  if (!this->keyword_.empty())
  {
    book_modify_package.keyword_ = this->keyword_;
  }
  if (this->price_ != -1)
  {
    book_modify_package.price_ = this->price_;
  }
  BookManager::getInstance().Modify(book_modify_package);
}
void ImportStatement::Execute()
{
  if (AccountManager::getInstance().GetCurrentPrivilege() < 3) // 权限检查
  {
    throw std::runtime_error("Invalid\n");
  }
  BookManager::getInstance().Import(quantity_, total_cost_);
}
void ShowFinanceStatement::Execute()
{
  if (AccountManager::getInstance().GetCurrentPrivilege() < 7) // 权限检查
  {
    throw std::runtime_error("Invalid\n");
  }
  if (count_ == -1)
  {
    FinanceManager::getInstance().ShowAllFinanceReport();
    return;
  }
  else
  {
    FinanceManager::getInstance().ShowFinanceReport(count_);
  }
}
void LogStatement::Execute() 
{
  if(AccountManager::getInstance().GetCurrentPrivilege() != 7)
  {
    throw std::runtime_error("Invalid\n");
  }
  LogManager::getInstance().Log();
}
void ReportFinanceStatement::Execute() 
{
  if(AccountManager::getInstance().GetCurrentPrivilege() != 7)
  {
    throw std::runtime_error("Invalid\n");
  }
  FinanceManager::getInstance().ReportFinance();
}
void ReportEmployeeStatement::Execute()
{
  if(AccountManager::getInstance().GetCurrentPrivilege() != 7)
  {
    throw std::runtime_error("Invalid\n");
  }
  LogManager::getInstance().ReportEmployee();
}
