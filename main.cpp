#include <string>
#include <vector>
#include <iostream>
#include "./el/token.hpp"

int main() {
  std::cout << "====start====" << std::endl;

  cppel::Tokenizer tokenizer("(('你好' + ' ') + 'world')");

  std::vector<cppel::Token> tokens;

  while (tokenizer.peek_token().kind_ != cppel::Token::Kind::END)
  {
    tokens.push_back(tokenizer.next_token());
  }
  

  return 0;
}