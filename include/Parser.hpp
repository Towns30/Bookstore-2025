#ifndef PARSER_HPP
#define PARSER_HPP
#include "Statement.hpp"
#include <string>

class Parser
{
public:
  Statement *ParseLine(std::string text);

private:
  Statement *ParseToSu(std::string text);
  Statement *ParseToLogout(std::string text);
  Statement *ParseToRegister(std::string text);
  Statement *ParseToPasswd(std::string text);
  Statement *ParseToUserAdd(std::string text);
  Statement *ParseToDelete(std::string text);
  Statement *ParseToShowBook(std::string text);
  Statement *ParseToBuy(std::string text);
  Statement *ParseToSelect(std::string text);
  Statement *ParseToModify(std::string text);
  Statement *ParseToImport(std::string text);
  Statement *ParseToShowFinance(std::string text);
  Statement *ParseTolog(std::string text);
  Statement *ParseToReportFinance(std::string text);
  Statement *ParseToReportEmployee(std::string text);
};

#endif