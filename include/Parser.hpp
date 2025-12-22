#ifndef PARSER_HPP
#define PARSER_HPP
#include "Lexer.hpp"
#include "Statement.hpp"
#include <memory>
#include <string>
#include <vector>

class Parser
{
public:
  static std::unique_ptr<Statement> ParseLine(const std::vector<Token> &tokens);

private:
  static std::unique_ptr<Statement> ParseToSu(const std::vector<Token> &tokens);
  static std::unique_ptr<Statement> ParseToLogout(const std::vector<Token> &tokens);
  static std::unique_ptr<Statement> ParseToRegister(const std::vector<Token> &tokens);
  static std::unique_ptr<Statement> ParseToPasswd(const std::vector<Token> &tokens);
  static std::unique_ptr<Statement> ParseToUserAdd(const std::vector<Token> &tokens);
  static std::unique_ptr<Statement> ParseToDelete(const std::vector<Token> &tokens);
  static std::unique_ptr<Statement> ParseToShowBook(const std::vector<Token> &tokens);
  static std::unique_ptr<Statement> ParseToBuy(const std::vector<Token> &tokens);
  static std::unique_ptr<Statement> ParseToSelect(const std::vector<Token> &tokens);
  static std::unique_ptr<Statement> ParseToModify(const std::vector<Token> &tokens);
  static std::unique_ptr<Statement> ParseToImport(const std::vector<Token> &tokens);
  static std::unique_ptr<Statement> ParseToShowFinance(const std::vector<Token> &tokens);
  static std::unique_ptr<Statement> ParseToLog(const std::vector<Token> &tokens);
  static std::unique_ptr<Statement> ParseToReportFinance(const std::vector<Token> &tokens);
  static std::unique_ptr<Statement> ParseToReportEmployee(const std::vector<Token> &tokens);
};

#endif