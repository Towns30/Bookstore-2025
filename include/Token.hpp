#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <vector>

enum class TokenType
{
  BASIC,     // 基础标识符（无引号）
  PARAMETER, // 参数型 (前缀=值，例如 -ISBN=978...)
};
struct Token
{
  TokenType type_;
  std::string value_;  // 存储原始字符串或去掉引号后的内容
  std::string prefix_; // 专门存储参数前缀，如 "ISBN", "author"
};
#endif