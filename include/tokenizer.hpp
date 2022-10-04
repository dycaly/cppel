#pragma once

#include <string>
#include <vector>
#include <queue>
#include "exception.hpp"

namespace cppel {

class Token {
 public:
  enum class Kind {
    LITERAL_BOOL,    // bool
    LITERAL_INT,     // int
    LITERAL_FLOAT,   // float
    LITERAL_STRING,  // string
    IDENTIFIER,      // id
    LPAREN,          // (
    RPAREN,          // )
    LSQUARE,         // [
    RSQUARE,         // ]
    LCURLY,          // {
    RCURLY,          // }
    COMMA,           // ,
    COLON,           // :
    HASH,            // #
    DOT,             // .
    PLUS,            // +
    MINUS,           // -
    STAR,            // *
    POWER,           // ^
    DIV,             // /
    MOD,             // %
    QMARK,           // ?
    GT,              // >
    GE,              // >=
    LT,              // <
    LE,              // <=
    EQ,              // ==
    NE,              // !=
    NOT,             // !
    AND,             // &&
    OR,              // ||
    SELECT,          // ?[
    SELECT_FIRST,    // ^[
    SELECT_LAST,     // $[
    PROJECT,         // ![
    ELVIS,           // ?:
    SAFE_NAVI,       // ?.
    ASSIGN,          // =
    END,             // \0
  };

  Kind kind_;
  size_t start_pos_;
  size_t end_pos_;

  Token(const Kind kind, const size_t start_pos, const size_t end_pos)
      : kind_(kind), start_pos_(start_pos), end_pos_(end_pos) {}

  Token(const Token &other)
      : kind_(other.kind_),
        start_pos_(other.start_pos_),
        end_pos_(other.end_pos_) {}

  bool is_numeric_relation_operator() {
    return kind_ == Kind::GT || kind_ == Kind::GE || kind_ == Kind::LT || kind_ == Kind::LE || kind_ == Kind::EQ || kind_ == Kind::NE;
  }
};

class Tokenizer {
 public:
  Tokenizer(const std::string &expr_str) : expr_str_(expr_str) {
    for (char ch : expr_str) {
      expr_chars_.push_back(ch);
    }
    expr_chars_.push_back('\0');
    pos_ = 0;
    size_ = expr_chars_.size();
  }

  Token &peek_token() {
    scan_token();
    return token_queue_.front();
  }

  Token next_token() {
    scan_token();
    Token token = token_queue_.front();
    if (token.kind_ != Token::Kind::END) {
      token_queue_.pop();
    }
    return token;
  }

 private:
  std::string expr_str_;
  std::vector<char> expr_chars_;
  size_t pos_;
  size_t size_;
  std::queue<Token> token_queue_;

  void scan_token() {
    while (token_queue_.empty()) {
      if (pos_ >= size_) {
        CPPEL_THROW(TokenError("expression is end!"));
      }

      if (std::isalpha(expr_chars_[pos_])) {
        lex_identifier();
      } else if (std::isalnum(expr_chars_[pos_])) {
        lex_numeric_literal();
      } else if (std::isblank(expr_chars_[pos_])) {
        ++pos_;
      } else {
        switch (expr_chars_[pos_]) {
          case '\'':lex_quoted_string_literal();
            break;
          case '"':lex_double_quoted_string_literal();
            break;
          case '_':lex_identifier();
            break;
          case '(':enqueue_token(Token::Kind::LPAREN, 1);
            break;
          case ')':enqueue_token(Token::Kind::RPAREN, 1);
            break;
          case '[':enqueue_token(Token::Kind::LSQUARE, 1);
            break;
          case ']':enqueue_token(Token::Kind::RSQUARE, 1);
            break;
          case '{':enqueue_token(Token::Kind::LCURLY, 1);
            break;
          case '}':enqueue_token(Token::Kind::RCURLY, 1);
            break;
          case '+':enqueue_token(Token::Kind::PLUS, 1);
            break;
          case '-':enqueue_token(Token::Kind::MINUS, 1);
            break;
          case '*':enqueue_token(Token::Kind::STAR, 1);
            break;
          case '^':
            if (expr_chars_[pos_ + 1] == '[') {
              enqueue_token(Token::Kind::SELECT_FIRST, 2);
            } else {
              enqueue_token(Token::Kind::POWER, 1);
            }
            break;
          case '/':enqueue_token(Token::Kind::DIV, 1);
            break;
          case '%':enqueue_token(Token::Kind::MOD, 1);
            break;
          case ':':enqueue_token(Token::Kind::COLON, 1);
            break;
          case '#':enqueue_token(Token::Kind::HASH, 1);
            break;
          case '.':enqueue_token(Token::Kind::DOT, 1);
            break;
          case ',':enqueue_token(Token::Kind::COMMA, 1);
            break;
          case '?':
            if (expr_chars_[pos_ + 1] == '[') {
              enqueue_token(Token::Kind::SELECT, 2);
            } else if (expr_chars_[pos_ + 1] == ':') {
              enqueue_token(Token::Kind::ELVIS, 2);
            } else if (expr_chars_[pos_ + 1] == '.') {
              enqueue_token(Token::Kind::SAFE_NAVI, 2);
            } else {
              enqueue_token(Token::Kind::QMARK, 1);
            }
            break;
          case '>':
            if (expr_chars_[pos_ + 1] == '=') {
              enqueue_token(Token::Kind::GE, 2);
            } else {
              enqueue_token(Token::Kind::GT, 1);
            }
            break;
          case '<':
            if (expr_chars_[pos_ + 1] == '=') {
              enqueue_token(Token::Kind::LE, 2);
            } else {
              enqueue_token(Token::Kind::LT, 1);
            }
            break;
          case '=':
            if (expr_chars_[pos_ + 1] == '=') {
              enqueue_token(Token::Kind::EQ, 2);
            } else {
              enqueue_token(Token::Kind::ASSIGN, 1);
            }
            break;
          case '!':
            if (expr_chars_[pos_ + 1] == '=') {
              enqueue_token(Token::Kind::NE, 2);
            } else if (expr_chars_[pos_ + 1] == '[') {
              enqueue_token(Token::Kind::PROJECT, 2);
            } else {
              enqueue_token(Token::Kind::NOT, 1);
            }
            break;
          case '|':
            if (expr_chars_[pos_ + 1] == '|') {
              enqueue_token(Token::Kind::OR, 2);
            } else {
              throw_unexcept_char();
            }
            break;
          case '&':
            if (expr_chars_[pos_ + 1] == '&') {
              enqueue_token(Token::Kind::AND, 2);
            } else {
              throw_unexcept_char();
            }
            break;
          case '$':
            if (expr_chars_[pos_ + 1] == '[') {
              enqueue_token(Token::Kind::SELECT_LAST, 2);
            } else {
              throw_unexcept_char();
            }
            break;
          case '\0':enqueue_token(Token::Kind::END, 1);
            break;
          default:throw_unexcept_char();
            break;
        }
      }
    }
  }

  void lex_identifier() {
    size_t start = pos_;
    while (is_identifier_char(expr_chars_[pos_])) ++pos_;
    std::string identifier = expr_str_.substr(start, pos_ - start);
    if (identifier == "true" || identifier == "false") {
      token_queue_.emplace(Token::Kind::LITERAL_BOOL, start, pos_);
    } else if (identifier == "not") {
      token_queue_.emplace(Token::Kind::NOT, start, pos_);
    } else if (identifier == "and") {
      token_queue_.emplace(Token::Kind::AND, start, pos_);
    } else if (identifier == "or") {
      token_queue_.emplace(Token::Kind::OR, start, pos_);
    } else {
      token_queue_.emplace(Token::Kind::IDENTIFIER, start, pos_);
    }
  }

  bool is_identifier_char(char ch) {
    return std::isalpha(ch) || std::isalnum(ch) || ch == '_';
  }

  void lex_numeric_literal() {
    size_t start = pos_;

    while (std::isalnum(expr_chars_[pos_])) ++pos_;
    if (expr_chars_[pos_] == '.') {
      ++pos_;
      while (std::isalnum(expr_chars_[pos_])) ++pos_;
      token_queue_.emplace(Token::Kind::LITERAL_FLOAT, start, pos_);
    } else {
      token_queue_.emplace(Token::Kind::LITERAL_INT, start, pos_);
    }
  }

  void lex_quoted_string_literal() {
    size_t start = pos_;
    ++pos_;
    while (expr_chars_[pos_] != '\'') ++pos_;
    ++pos_;
    token_queue_.emplace(Token::Kind::LITERAL_STRING, start, pos_);
  }

  void lex_double_quoted_string_literal() {
    size_t start = pos_;
    ++pos_;
    while (expr_chars_[pos_] != '"') ++pos_;
    ++pos_;
    token_queue_.emplace(Token::Kind::LITERAL_STRING, start, pos_);
  }

  void enqueue_token(const Token::Kind kind, const size_t size) {
    token_queue_.emplace(kind, pos_, pos_ + size);
    pos_ += size;
  }

  void throw_unexcept_char() {
    CPPEL_THROW("Unexcept char '" + std::to_string(expr_chars_[pos_]) +
        "' at pos " + std::to_string(pos_));
  }
};

}  // namespace cppel
