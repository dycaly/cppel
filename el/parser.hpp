#pragma once

#include <string>
#include <memory>
#include <vector>
#include <deque>
#include "exception.hpp"
#include "tokenizer.hpp"
#include "ast.hpp"

namespace cppel {

class Parser {
 public:
  Parser() : tokenizer_(nullptr) {}

  std::shared_ptr<AstNode> parse(const std::string &expr_str) {
    tokenizer_ = std::make_shared<Tokenizer>(expr_str);
    return eat_expression();
  }

 private:
  std::shared_ptr<Tokenizer> tokenizer_;
  std::deque<std::shared_ptr<AstNode>> constructedNodes_;

  Token &peek_token() {
    return tokenizer_->peek_token();
  }

  Token next_token() {
    return tokenizer_->next_token();
  }

  std::shared_ptr<AstNode> eat_expression() {
    std::shared_ptr<AstNode> expr = eat_logical_or_expression();
    Token &token = peek_token();
    if (token.kind_ == Token::Kind::ASSIGN) {
      if (!expr) {
        return std::make_shared<LiteralNone>(token.start_pos_ - 1, token.end_pos_ - 1);
      }
      next_token();
      std::shared_ptr<AstNode> assigned_value = eat_logical_or_expression();
      return std::make_shared<Assign>(token.start_pos_, token.end_pos_, expr, assigned_value);
    } else if (token.kind_ == Token::Kind::ELVIS) {
      if (!expr) {
        return std::make_shared<LiteralNone>(token.start_pos_ - 1, token.end_pos_ - 1);
      }
      next_token();
      std::shared_ptr<AstNode> else_value = eat_expression();
      return std::make_shared<Elvis>(token.start_pos_, token.end_pos_, expr, else_value);
    } else if (token.kind_ == Token::Kind::QMARK) {
      if (!expr) {
        return std::make_shared<LiteralNone>(token.start_pos_ - 1, token.end_pos_ - 1);
      }
      next_token();
      std::shared_ptr<AstNode> if_true_value = eat_expression();


    }

    return expr;
  }

  std::shared_ptr<AstNode> eat_logical_or_expression() {
    return nullptr;
  }

};

} // namespace cppel
