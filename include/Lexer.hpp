#ifndef LEXER_HPP
#define LEXER_HPP
#include "Token.hpp"
#include <vector>

class Lexer
{
public:
  static std::vector<Token> Tokenize(const std::string &line);

private:
  static void SkipBlank(const std::string &line, int &pos); // 跳过空格
  static Token GetBasic(const std::string &line, int &pos);
  static Token GetParameter(const std::string &line, int &pos);
  static std::string GetQuoted(const std::string &line, int &pos);
};
#endif