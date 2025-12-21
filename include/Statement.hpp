#ifndef STATEMENT_HPP
#define STATEMENT_HPP
#include <string>

class Statement
{
public:
  Statement() {};
  ~Statement() = default;
  virtual void Execute() = 0;
};
class SuStatement : public Statement
{
public:
  void Execute();
  SuStatement(std::string user_id, std::string passwd) : user_id_(user_id), passwd_(passwd) {}

private:
  std::string user_id_;
  std::string passwd_;
};
class LogoutStatement : public Statement
{
public:
  void Execute();
  LogoutStatement() {}
};
class RegisterStatement : public Statement
{
public:
  void Execute();
  RegisterStatement(std::string user_id, std::string passwd, std::string user_name) : user_id_(user_id), passwd_(passwd), user_name_(user_name){}

private:
  std::string user_id_;
  std::string passwd_;
  std::string user_name_;
};
class PasswdStatement : public Statement
{
public:
  void Execute();
  PasswdStatement(std::string user_id, std::string cur_passwd, std::string new_passwd)
      : user_id_(user_id), cur_passwd_(cur_passwd), new_passwd_(new_passwd)
  {
  }

private:
  std::string user_id_;
  std::string cur_passwd_;
  std::string new_passwd_;
};
class UserAddStatement : public Statement
{
public:
  void Execute();
  UserAddStatement(std::string user_id, std::string passwd, int privilege, std::string user_name)
      : user_id_(user_id), passwd_(passwd), privilege_(privilege), user_name_(user_name)
  {
  }

private:
  std::string user_id_;
  std::string passwd_;
  int privilege_;
  std::string user_name_;
};
class DeleteStatement : public Statement
{
public:
  void Execute();
  DeleteStatement(std::string user_id) : user_id_(user_id) {}

private:
  std::string user_id_;
};
class ShowBookStatement : public Statement
{
public:
  void Execute();
  ShowBookStatement(std::string isbn, std::string bookname, std::string author, std::string keyword,
                    bool all_show = false)
      : isbn_(isbn), bookname_(bookname), author_(author), keyword_(keyword), all_show_(all_show)
  {
  }

private:
  std::string isbn_;
  std::string bookname_;
  std::string author_;
  std::string keyword_;
  bool all_show_;
};
class BuyStatement : public Statement
{
public:
  void Execute();
  BuyStatement(std::string isbn, int quantity) : isbn_(isbn), quantity_(quantity) {}

private:
  std::string isbn_;
  int quantity_;
};
class SelectStatement : public Statement
{
public:
  void Execute();
  SelectStatement(std::string isbn) : isbn_(isbn) {}

private:
  std::string isbn_;
};
class ModifyStatement : public Statement
{
public:
  void Execute();
  ModifyStatement(std::string isbn, std::string bookname, std::string author, std::string keyword,
                  long long price = -1)
      : isbn_(isbn), bookname_(bookname), author_(author), keyword_(keyword), price_(price)
  {
  }

private:
  std::string isbn_;
  std::string bookname_;
  std::string author_;
  std::string keyword_;
  long long price_; // price * 100
};
class ImportStatement : public Statement
{
public:
  void Execute();
  ImportStatement(int quantity, long long total_cost) : quantity_(quantity), total_cost_(total_cost)
  {
  }

private:
  int quantity_;
  long long total_cost_; // totalcost * 100
};
class ShowFinanceStatement : public Statement
{
public:
  void Execute();
  ShowFinanceStatement(int count = -1) : count_(count) {}

private:
  int count_;
};
class LogStatement : public Statement
{
public:
  void Execute();
  LogStatement() {}
};
class ReportFinanceStatement : public Statement
{
public:
  void Execute();
  ReportFinanceStatement() {}
};
class ReportEmployeeStatement : public Statement
{
public:
  void Execute();
  ReportEmployeeStatement() {}
};

#endif
