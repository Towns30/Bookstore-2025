#include "../include/Parser.hpp"
#include "../include/Lexer.hpp"
#include "../include/Managers.hpp"
#include "../include/Utils.hpp"
#include <stdexcept>

std::unique_ptr<Statement> Parser::ParseLine(const std::vector<Token> &tokens)
{
  if (tokens.empty()) // 空行
  {
    return nullptr;
  }
  auto token = tokens[0];
  TokenType type = token.type_;
  std::string command = token.value_;
  if (type != TokenType::BASIC) // 若第一个不是BASIC，则错误
  {
    throw std::runtime_error("Invalid\n");
  }
  if (command == "su")
  {
    return ParseToSu(tokens);
  }
  if (command == "logout")
  {
    return ParseToLogout(tokens);
  }
  if (command == "register")
  {
    return ParseToRegister(tokens);
  }
  if (command == "passwd")
  {
    return ParseToPasswd(tokens);
  }
  if (command == "useradd")
  {
    return ParseToUserAdd(tokens);
  }
  if (command == "delete")
  {
    return ParseToDelete(tokens);
  }
  if (command == "show" && tokens.size() >= 2 && tokens[1].type_ == TokenType::BASIC &&
      tokens[1].value_ == "finance")
  {
    return ParseToShowFinance(tokens);
  }
  if (command == "buy")
  {
    return ParseToBuy(tokens);
  }
  if (command == "select")
  {
    return ParseToSelect(tokens);
  }
  if (command == "modify")
  {
    return ParseToModify(tokens);
  }
  if (command == "import")
  {
    return ParseToImport(tokens);
  }
  if (command == "show")
  {
    return ParseToShowBook(tokens);
  }
  if (command == "log")
  {
    return ParseToLog(tokens);
  }
  if (command == "report" && tokens.size() == 2 && tokens[1].type_ == TokenType::BASIC &&
      tokens[1].value_ == "finance")
  {
    return ParseToReportFinance(tokens);
  }
  if (command == "report" && tokens.size() == 2 && tokens[1].type_ == TokenType::BASIC &&
      tokens[1].value_ == "employee")
  {
    return ParseToReportEmployee(tokens);
  }
  throw std::runtime_error("Invalid\n");
}
std::unique_ptr<Statement> Parser::ParseToSu(const std::vector<Token> &tokens)
{
  if (tokens.size() != 2 && tokens.size() != 3)
  {
    throw std::runtime_error("Invalid\n");
  }
  if (tokens[1].type_ != TokenType::BASIC)
  {
    throw std::runtime_error("Invalid\n");
  }
  std::string user_id = tokens[1].value_;
  Utils::CheckUserID(user_id);
  if (tokens.size() == 2)
  {
    return std::make_unique<SuStatement>(user_id, "");
  }

  if (tokens[2].type_ != TokenType::BASIC)
  {
    throw std::runtime_error("Invalid\n");
  }
  std::string passwd = tokens[2].value_;
  Utils::CheckPasswd(passwd);
  return std::make_unique<SuStatement>(user_id, passwd);
}
std::unique_ptr<Statement> Parser::ParseToLogout(const std::vector<Token> &tokens)
{
  if (tokens.size() != 1)
  {
    throw std::runtime_error("Invalid\n");
  }
  return std::make_unique<LogoutStatement>();
}
std::unique_ptr<Statement> Parser::ParseToRegister(const std::vector<Token> &tokens)
{
  if (tokens.size() != 4)
  {
    throw std::runtime_error("Invalid\n");
  }
  if (tokens[1].type_ != TokenType::BASIC)
  {
    throw std::runtime_error("Invalid\n");
  }
  std::string user_id = tokens[1].value_;
  Utils::CheckUserID(user_id);
  if (tokens[2].type_ != TokenType::BASIC)
  {
    throw std::runtime_error("Invalid\n");
  }
  std::string passwd = tokens[2].value_;
  Utils::CheckPasswd(passwd);
  if (tokens[3].type_ != TokenType::BASIC)
  {
    throw std::runtime_error("Invalid\n");
  }
  std::string user_name = tokens[3].value_;
  Utils::CheckUserName(user_name);
  return std::make_unique<RegisterStatement>(user_id, passwd, user_name);
}
std::unique_ptr<Statement> Parser::ParseToPasswd(const std::vector<Token> &tokens)
{
  if (tokens.size() != 3 && tokens.size() != 4)
  {
    throw std::runtime_error("Invalid\n");
  }
  if (tokens[1].type_ != TokenType::BASIC)
  {
    throw std::runtime_error("Invalid\n");
  }
  std::string user_id = tokens[1].value_;
  Utils::CheckUserID(user_id);
  if (tokens.size() == 3)
  {
    if (tokens[2].type_ != TokenType::BASIC)
    {
      throw std::runtime_error("Invalid\n");
    }
    std::string new_passwd = tokens[2].value_;
    Utils::CheckPasswd(new_passwd);
    return std::make_unique<PasswdStatement>(user_id, "", new_passwd);
  }
  else
  {
    if (tokens[2].type_ != TokenType::BASIC)
    {
      throw std::runtime_error("Invalid\n");
    }
    std::string cur_passwd = tokens[2].value_;
    Utils::CheckPasswd(cur_passwd);
    if (tokens[3].type_ != TokenType::BASIC)
    {
      throw std::runtime_error("Invalid\n");
    }
    std::string new_passwd = tokens[3].value_;
    Utils::CheckPasswd(new_passwd);
    return std::make_unique<PasswdStatement>(user_id, cur_passwd, new_passwd);
  }
}
std::unique_ptr<Statement> Parser::ParseToUserAdd(const std::vector<Token> &tokens)
{
  if (tokens.size() != 5)
  {
    throw std::runtime_error("Invalid\n");
  }
  if (tokens[1].type_ != TokenType::BASIC)
  {
    throw std::runtime_error("Invalid\n");
  }
  std::string user_id = tokens[1].value_;
  Utils::CheckUserID(user_id);
  if (tokens[2].type_ != TokenType::BASIC)
  {
    throw std::runtime_error("Invalid\n");
  }
  std::string passwd = tokens[2].value_;
  Utils::CheckPasswd(passwd);
  if (tokens[3].type_ != TokenType::BASIC)
  {
    throw std::runtime_error("Invalid\n");
  }
  std::string privilege = tokens[3].value_;
  Utils::CheckPrivilege(privilege);
  int privilege_int = privilege[0] - '0';
  if (tokens[4].type_ != TokenType::BASIC)
  {
    throw std::runtime_error("Invalid\n");
  }
  std::string user_name = tokens[4].value_;
  Utils::CheckUserName(user_name);
  return std::make_unique<UserAddStatement>(user_id, passwd, privilege_int, user_name);
}
std::unique_ptr<Statement> Parser::ParseToDelete(const std::vector<Token> &tokens)
{
  if (tokens.size() != 2)
  {
    throw std::runtime_error("Invalid\n");
  }
  if (tokens[1].type_ != TokenType::BASIC)
  {
    throw std::runtime_error("Invalid\n");
  }
  std::string user_id = tokens[1].value_;
  Utils::CheckUserID(user_id);
  return std::make_unique<DeleteStatement>(user_id);
}
std::unique_ptr<Statement> Parser::ParseToShowBook(const std::vector<Token> &tokens)
{
  if (tokens.size() != 1 && tokens.size() != 2)
  {
    throw std::runtime_error("Invalid\n");
  }
  if (tokens.size() == 1)
  {
    return std::make_unique<ShowBookStatement>("", "", "", "", true);
  }
  else
  {
    if (tokens[1].type_ != TokenType::PARAMETER)
    {
      throw std::runtime_error("Invalid\n");
    }
    if (tokens[1].prefix_ == "ISBN")
    {
      std::string isbn = tokens[1].value_;
      Utils::CheckISBN(isbn);
      return std::make_unique<ShowBookStatement>(isbn, "", "", "", false);
    }
    if (tokens[1].prefix_ == "name")
    {
      std::string bookname = tokens[1].value_;
      Utils::CheckBookName(bookname);
      return std::make_unique<ShowBookStatement>("", bookname, "", "", false);
    }
    if (tokens[1].prefix_ == "author")
    {
      std::string author = tokens[1].value_;
      Utils::CheckAuthor(author);
      return std::make_unique<ShowBookStatement>("", "", author, "", false);
    }
    if (tokens[1].prefix_ == "keyword")
    {
      std::string keyword = tokens[1].value_;
      Utils::CheckKeyword(keyword);
      return std::make_unique<ShowBookStatement>("", "", "", keyword, false);
    }
    else
    {
      throw std::runtime_error("Invalid\n");
    }
  }
}
std::unique_ptr<Statement> Parser::ParseToBuy(const std::vector<Token> &tokens)
{
  if (tokens.size() != 3)
  {
    throw std::runtime_error("Invalid\n");
  }
  if (tokens[1].type_ != TokenType::BASIC)
  {
    throw std::runtime_error("Invalid\n");
  }
  std::string isbn = tokens[1].value_;
  Utils::CheckISBN(isbn);
  if (tokens[2].type_ != TokenType::BASIC)
  {
    throw std::runtime_error("Invalid\n");
  }
  std::string quantity = tokens[2].value_;
  Utils::CheckQuantity(quantity);
  int quantity_int = Utils::StringToInt(quantity);
  if (quantity_int <= 0) // 购买数量为非正整数则操作失败
  {
    throw std::runtime_error("Invalid\n");
  }
  return std::make_unique<BuyStatement>(isbn, quantity_int);
}
std::unique_ptr<Statement> Parser::ParseToSelect(const std::vector<Token> &tokens)
{
  if (tokens.size() != 2)
  {
    throw std::runtime_error("Invalid\n");
  }
  if (tokens[1].type_ != TokenType::BASIC)
  {
    throw std::runtime_error("Invalid\n");
  }
  std::string isbn = tokens[1].value_;
  Utils::CheckISBN(isbn);
  return std::make_unique<SelectStatement>(isbn);
}
std::unique_ptr<Statement> Parser::ParseToModify(const std::vector<Token> &tokens) 
{
  if(tokens.size() < 2 || tokens.size() > 6)
  {
    throw std::runtime_error("Invalid\n");
  }
  bool vis[5] = {false, false, false, false, false};
  std::string isbn = "";
  std::string bookname = "";
  std::string author = "";
  std::string keywords = "";
  std::string price = "";
  long long price_ll = -1;
  for (int i = 1; i <= tokens.size() - 1; i++)
  {
    if (tokens[i].type_ != TokenType::PARAMETER)
    {
      throw std::runtime_error("Invalid\n");
    }
    std::string prefix = tokens[i].prefix_;
    if(prefix == "ISBN")
    {
      if(vis[0]) // 有重复附加参数为非法指令
      {
        throw std::runtime_error("Invalid\n");
      }
      isbn = tokens[i].value_;
      Utils::CheckISBN(isbn);
      vis[0] = true;
    }
    else if(prefix == "name")
    {
      if(vis[1]) // 有重复附加参数为非法指令
      {
        throw std::runtime_error("Invalid\n");
      }
      bookname = tokens[i].value_;
      Utils::CheckBookName(bookname);
      vis[1] = true;
    }
    else if(prefix == "author")
    {
      if(vis[2]) // 有重复附加参数为非法指令
      {
        throw std::runtime_error("Invalid\n");
      }
      author = tokens[i].value_;
      Utils::CheckAuthor(author);
      vis[2] = true;
    }
    else if(prefix == "keyword")
    {
      if(vis[3]) // 有重复附加参数为非法指令
      {
        throw std::runtime_error("Invalid\n");
      }
      keywords = tokens[i].value_;
      Utils::CheckKeywords(keywords);
      vis[3] = true;
    }
    else if(prefix == "price")
    {
      if(vis[4]) // 有重复附加参数为非法指令
      {
        throw std::runtime_error("Invalid\n");
      }
      price = tokens[i].value_;
      price_ll = Utils::CheckPrice(price);
      vis[4] = true;
    }
    else
    {
      throw std::runtime_error("Invalid\n");
    }
  }
  return std::make_unique<ModifyStatement>(isbn, bookname, author, keywords, price_ll);
}
std::unique_ptr<Statement> Parser::ParseToImport(const std::vector<Token> &tokens)
{
  if (tokens.size() != 3)
  {
    throw std::runtime_error("Invalid\n");
  }
  if (tokens[1].type_ != TokenType::BASIC)
  {
    throw std::runtime_error("Invalid\n");
  }
  std::string quantity = tokens[1].value_;
  Utils::CheckQuantity(quantity);
  int quantity_int = Utils::StringToInt(quantity);
  if (quantity_int <= 0) // 购入数量为非正整数则操作失败
  {
    throw std::runtime_error("Invalid\n");
  }
  if (tokens[2].type_ != TokenType::BASIC)
  {
    throw std::runtime_error("Invalid\n");
  }
  std::string total_cost = tokens[2].value_;
  long long total_cost_ll = Utils::CheckTotalCost(total_cost);
  if (total_cost_ll <= 0) // 交易总额为非正数则操作失败
  {
    throw std::runtime_error("Invalid\n");
  }
  return std::make_unique<ImportStatement>(quantity_int, total_cost_ll);
}
std::unique_ptr<Statement> Parser::ParseToShowFinance(const std::vector<Token> &tokens)
{
  if (tokens.size() != 2 && tokens.size() != 3)
  {
    throw std::runtime_error("Invalid\n");
  }
  if (tokens.size() == 2)
  {
    return std::make_unique<ShowFinanceStatement>(-1);
  }
  else
  {
    if (tokens[2].type_ != TokenType::BASIC)
    {
      throw std::runtime_error("Invalid\n");
    }
    std::string count = tokens[2].value_;
    Utils::CheckCount(count);
    int count_int = Utils::StringToInt(count);
    return std::make_unique<ShowFinanceStatement>(count_int);
  }
}
std::unique_ptr<Statement> Parser::ParseToLog(const std::vector<Token> &tokens) 
{
  if(tokens.size() != 1)
  {
    throw std::runtime_error("Invalid\n");
  }
  return std::make_unique<LogStatement>();
}
std::unique_ptr<Statement> Parser::ParseToReportFinance(const std::vector<Token> &tokens) 
{
  if(tokens.size() != 2)
  {
    throw std::runtime_error("Invalid\n");
  }
  return std::make_unique<ReportFinanceStatement>();
}
std::unique_ptr<Statement> Parser::ParseToReportEmployee(const std::vector<Token> &tokens) 
{
  if(tokens.size() != 2)
  {
    throw std::runtime_error("Invalid\n");
  }
  return std::make_unique<ReportEmployeeStatement>();
}