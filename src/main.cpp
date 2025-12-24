#include "../include/Lexer.hpp"
#include "../include/Managers.hpp"
#include "../include/Parser.hpp"
#include <iostream>
#include <string>
#include <vector>

int main()
{
  std::string line;
  Lexer lexer;
  Parser parser;
  while (std::getline(std::cin, line))
  {
    if (line.empty()) // 仅有空格的指令合法且无输出内容
    {
      continue;
    }
    try
    {
      std::vector<Token> tokens = lexer.Tokenize(line);
      if (tokens.size() == 1 && tokens[0].type_ == TokenType::BASIC &&
          (tokens[0].value_ == "quit" || tokens[0].value_ == "exit"))
      {
        return 0;
      }
      auto statement = parser.ParseLine(tokens);
      LogManager::getInstance().AddSource(line);
      if (statement)
      {
        statement->Execute();
      }
      LogManager::getInstance().SetResult();
    }
    catch (const std::runtime_error &e)
    {
      std::cout << e.what();
    }
  }
  return 0;
}