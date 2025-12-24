#include "../include/Lexer.hpp"
#include <iostream>
#include <stdexcept>

std::vector<Token> Lexer::Tokenize(const std::string &line)
{
  int pos = 0;
  std::vector<Token> tokens;
  while(pos <= line.size() - 1)
  {
    SkipBlank(line, pos);
    if(pos == line.size())
    {
      break;
    }
    if(line[pos] == '-')
    {
      tokens.push_back(GetParameter(line, pos));
    }
    else
    {
      tokens.push_back(GetBasic(line, pos));
    }
  }
  // std::cerr << "Great Lexer!" << std::endl;
  return tokens;
}
void Lexer::SkipBlank(const std::string &line, int &pos)
{
  while (pos <= (line.size() - 1) && line[pos] <= 32)
  {
    if (line[pos] <= 31)
    {
      // std::cout << "damn" << std::endl;
      throw std::runtime_error("Invalid\n");
    }
    pos++;
  }
}
Token Lexer::GetBasic(const std::string &line, int &pos)
{
  Token token;
  std::string basic;
  while(pos <= line.size() - 1 && line[pos] != ' ')
  {
    basic += line[pos];
    pos++;
  }
  // std::cerr << pos << std::endl;
  token.type_ = TokenType::BASIC;
  token.value_ = basic;
  return token;
}
Token Lexer::GetParameter(const std::string &line, int &pos)
{
  Token token;
  std::string prefix;
  pos++;
  while (pos <= line.size() - 1 && line[pos] != '=' && line[pos] != '\"')
  {
    prefix += line[pos];
    pos++;
  }
  if (pos == line.size())
  {
    throw std::runtime_error("Invalid\n");
  }
  if (prefix.empty())
  {
    throw std::runtime_error("Invalid\n");
  }
  token.type_ = TokenType::PARAMETER;
  token.prefix_ = prefix;
  if (line[pos] == '=')
  {
    pos++;
    if (line[pos] == '\"')
    {
      token.value_ = GetQuoted(line, pos);
    }
    else
    {
      token.value_ = GetBasic(line, pos).value_;
    }
  }
  return token;
}

std::string Lexer::GetQuoted(const std::string &line, int &pos)
{
  std::string quote = "";
  pos++;
  while (pos <= line.size() - 1 && line[pos] != '\"')
  {
    // std::cerr << pos << std::endl;
    quote += line[pos];
    pos++;
  }
  if (pos == line.size()) // 最后一个不是引号
  {
    throw std::runtime_error("Invalid\n");
  }
  else
  {
    pos++;           // 跳过右引号，防止skipblank把这个引号误判为关键字
    if (quote == "") // 引号内为空，错误
    {
      throw std::runtime_error("Invalid\n");
    }
    // std::cerr << quote << std::endl;
    return quote;
  }
}