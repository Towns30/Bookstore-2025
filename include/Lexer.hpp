#ifndef LEXER_HPP
#define LEXER_HPP
#include "Token.hpp"
#include <vector>

class Lexer
{
public:
  std::vector<Token> Tokenize(const std::string &line);

private:
  void SkipBlank(const std::string &line, int &pos); // 跳过空格
  Token GetBasic(const std::string &line, int &pos);
  Token GetParameter(const std::string &line, int &pos);
  std::string GetQuoted(const std::string &line, int &pos);
};
#endif