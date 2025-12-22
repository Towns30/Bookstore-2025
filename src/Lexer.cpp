#include "../include/Lexer.hpp"
#include <stdexcept>

std::vector<Token> Lexer::Tokenize(const std::string &line)
{
  int pos = 0;
  std::vector<Token> tokens;
  while (true)
  {
    SkipBlank(line, pos);
    if (pos >= line.size())
    {
      break;
    }
    auto current = line[pos];
    if (current == '-')
    {
      tokens.push_back(GetParameter(line, pos));
    }
    else
    {
      tokens.push_back(GetBasic(line, pos));
    }
  }
  return tokens;
}
void Lexer::SkipBlank(const std::string &line, int &pos)
{
  while (pos < line.size() && (unsigned char)line[pos] <= 32)
  {
    pos++;
  }
}
Token Lexer::GetBasic(const std::string &line, int &pos)
{
  Token token;
  token.type_ = TokenType::BASIC;
  int start = pos;
  while (pos < line.size() && (unsigned char)line[pos] > 32)
  {
    pos++;
  }
  token.value_ = line.substr(start, pos - start);
  return token;
}
Token Lexer::GetParameter(const std::string &line, int &pos)
{
  Token token;
  token.type_ = TokenType::PARAMETER;
  pos++; // 跳过'-'
  int start = pos;
  while (pos < line.size() && line[pos] != '=') // 记录，直到等号
  {
    if ((unsigned char)line[pos] <= 32 || (unsigned char)line[pos] > 126) // 如果前缀里面有空格，或TAB之类的
    {
      throw std::runtime_error("Invalid\n");
    }
    pos++;
  }
  token.prefix_ = line.substr(start, pos - start);
  if (line[pos] != '=') // 下一个不是'='
  {
    throw std::runtime_error("Invalid\n");
  }
  pos++;                  // 跳过'='
  if (pos == line.size()) // 等号后面啥都没有
  {
    throw std::runtime_error("Invalid\n");
  }
  if (line[pos] == '\"') // 下一个是引用内容
  {
    token.value_ = GetQuoted(line, pos);
  }
  else
  {
    int value_start = pos;
    while (pos < line.size() && (unsigned char)line[pos] > 32)
    {
      pos++;
    }
    token.value_ = line.substr(value_start, pos - value_start);
    if (token.value_ == "") // 等号后面啥都没有
    {
      throw std::runtime_error("Invalid\n");
    }
  }
  return token;
}

std::string Lexer::GetQuoted(const std::string &line, int &pos)
{
  pos++; // 跳过'\"'
  int start = pos;
  while (pos < line.size() && line[pos] != '\"')
  {
    pos++;
  }
  if (pos == line.size())
  {
    throw std::runtime_error("Invalid\n");
  }
  else
  {
    std::string get_quoted = line.substr(start, pos - start);
    pos++; // 跳过右引号，防止skipblank把这个引号误判为关键字
    if (get_quoted == "") // 引号内为空，错误
    {
      throw std::runtime_error("Invalid\n");
    }
    return get_quoted;
  }
}