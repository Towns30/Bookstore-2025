#ifndef STATEMENT_HPP
#define STATEMENT_HPP
#include <string>

class Statement
{
public:
  Statement();
  ~Statement() = default;
  virtual void Execute(std::string user_id);
};

#endif
