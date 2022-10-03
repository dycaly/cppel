#pragma once

#include <string>
#include <memory>
#include <vector>
#include <deque>
#include "exception.hpp"
#include "tokenizer.hpp"
#include "ast.hpp"
#include "expression.hpp"

namespace cppel {

class InternalParser {
 public:
  InternalParser(const std::string &expr_str) : expr_str_(expr_str), tokenizer_(expr_str) {}

  std::shared_ptr<AstNode> parse() {
    return eat_expression();
  }

 private:
  std::string expr_str_;
  Tokenizer tokenizer_;
  std::deque<std::shared_ptr<AstNode>> constructedNodes_;

  /**
   * handle expression
   *
   * @return
   */
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
      eat_token(Token::Kind::COLON);
      std::shared_ptr<AstNode> if_false_value = eat_expression();
      return std::make_shared<Ternary>(token.start_pos_, token.end_pos_, expr, if_true_value, if_false_value);
    }
    return expr;
  }

  /**
   * handle or
   * @return
   */
  std::shared_ptr<AstNode> eat_logical_or_expression() {
    std::shared_ptr<AstNode> expr = eat_logical_and_expression();
    while (peek_token(Token::Kind::OR)) {
      Token token = next_token();
      std::shared_ptr<AstNode> rh_expr = eat_logical_and_expression();
      if (!expr) {
        CPPEL_THROW(ParseError("unexpected null before " + token.start_pos_));
      }
      if (!rh_expr) {
        CPPEL_THROW(ParseError("unexpected null after " + token.start_pos_));
      }
      expr = std::make_shared<OpOr>(token.start_pos_, token.end_pos_, expr, rh_expr);
    }
    return expr;
  }

  /**
   * handle and
   * @return
   */
  std::shared_ptr<AstNode> eat_logical_and_expression() {
    std::shared_ptr<AstNode> expr = eat_relation_expression();
    while (peek_token(Token::Kind::AND)) {
      Token token = next_token();
      std::shared_ptr<AstNode> rh_expr = eat_relation_expression();
      if (!expr) {
        CPPEL_THROW(ParseError("unexpected null before " + token.start_pos_));
      }
      if (!rh_expr) {
        CPPEL_THROW(ParseError("unexpected null after " + token.start_pos_));
      }
      expr = std::make_shared<OpAnd>(token.start_pos_, token.end_pos_, expr, rh_expr);
    }
    return expr;
  }

  /**
   * handle relation like: >, >=, <, <=, ==, !=
   * @return
   */
  std::shared_ptr<AstNode> eat_relation_expression() {
    std::shared_ptr<AstNode> expr = eat_sum_expression();
    if (peek_token().is_numeric_relation_operator()) {
      Token token = next_token();
      std::shared_ptr<AstNode> rh_expr = eat_sum_expression();
      if (token.kind_ == Token::Kind::GT) {
        return std::make_shared<OpGT>(token.start_pos_, token.end_pos_, expr, rh_expr);
      } else if (token.kind_ == Token::Kind::GE) {
        return std::make_shared<OpGE>(token.start_pos_, token.end_pos_, expr, rh_expr);
      } else if (token.kind_ == Token::Kind::LT) {
        return std::make_shared<OpLT>(token.start_pos_, token.end_pos_, expr, rh_expr);
      } else if (token.kind_ == Token::Kind::LE) {
        return std::make_shared<OpLE>(token.start_pos_, token.end_pos_, expr, rh_expr);
      } else if (token.kind_ == Token::Kind::EQ) {
        return std::make_shared<OpEQ>(token.start_pos_, token.end_pos_, expr, rh_expr);
      } else {
        return std::make_shared<OpNE>(token.start_pos_, token.end_pos_, expr, rh_expr);
      }
    }
    return expr;
  }

  /**
   * handle +, -
   * @return
   */
  std::shared_ptr<AstNode> eat_sum_expression() {
    std::shared_ptr<AstNode> expr = eat_product_expression();
    while (peek_token().kind_ == Token::Kind::PLUS || peek_token().kind_ == Token::Kind::MINUS) {
      Token token = next_token();
      std::shared_ptr<AstNode> rh_expr = eat_product_expression();
      if (!rh_expr) {
        CPPEL_THROW(ParseError("unexpected null after " + token.start_pos_));
      }
      if (token.kind_ == Token::Kind::PLUS) {
        expr = std::make_shared<OpPlus>(token.start_pos_, token.end_pos_, expr, rh_expr);
      } else if (token.kind_ == Token::Kind::MINUS) {
        expr = std::make_shared<OpMinus>(token.start_pos_, token.end_pos_, expr, rh_expr);
      }
    }
    return expr;
  }

  /**
   * handle *, /, %
   * @return
   */
  std::shared_ptr<AstNode> eat_product_expression() {
    std::shared_ptr<AstNode> expr = eat_power_expression();
    while (peek_token().kind_ == Token::Kind::STAR || peek_token().kind_ == Token::Kind::DIV
        || peek_token().kind_ == Token::Kind::MOD) {
      Token token = next_token();
      std::shared_ptr<AstNode> rh_expr = eat_product_expression();
      if (!rh_expr) {
        CPPEL_THROW(ParseError("unexpected null after " + token.start_pos_));
      }
      if (token.kind_ == Token::Kind::STAR) {
        expr = std::make_shared<OpMultiply>(token.start_pos_, token.end_pos_, expr, rh_expr);
      } else if (token.kind_ == Token::Kind::DIV) {
        expr = std::make_shared<OpDivide>(token.start_pos_, token.end_pos_, expr, rh_expr);
      } else {
        expr = std::make_shared<OpModulus>(token.start_pos_, token.end_pos_, expr, rh_expr);
      }
    }
    return expr;
  }

  /**
   * handle ^
   * @return
   */
  std::shared_ptr<AstNode> eat_power_expression() {
    std::shared_ptr<AstNode> expr = eat_unary_expression();
    if (peek_token().kind_ == Token::Kind::POWER) {
      Token token = next_token();
      std::shared_ptr<AstNode> rh_expr = eat_unary_expression();
      if (!rh_expr) {
        CPPEL_THROW(ParseError("unexpected null after " + token.start_pos_));
      }
      return std::make_shared<OpPower>(token.start_pos_, token.end_pos_, expr, rh_expr);
    }
    return expr;
  }

  /**
   * handle unary -, -, !
   * @return
   */
  std::shared_ptr<AstNode> eat_unary_expression() {
    Token &pt = peek_token();
    if (pt.kind_ == Token::Kind::PLUS || pt.kind_ == Token::Kind::MINUS || pt.kind_ == Token::Kind::NOT) {
      Token token = next_token();
      std::shared_ptr<AstNode> expr = eat_unary_expression();
      if (!expr) {
        CPPEL_THROW(ParseError("unexpected null after " + token.start_pos_));
      }
      if (token.kind_ == Token::Kind::PLUS) {
        return std::make_shared<OpPlus>(token.start_pos_, token.end_pos_, expr, nullptr);
      } else if (token.kind_ == Token::Kind::MINUS) {
        return std::make_shared<OpMinus>(token.start_pos_, token.end_pos_, expr, nullptr);
      } else {
        return std::make_shared<OpNot>(token.start_pos_, token.end_pos_, expr);
      }
    }
    return eat_primary_expression();
  }

  /**
   * handle unary -, -, !
   * @return
   */
  std::shared_ptr<AstNode> eat_primary_expression() {
    std::shared_ptr<AstNode> start = eat_start_node();
    std::vector<std::shared_ptr<AstNode>> nodes;
    while (std::shared_ptr<AstNode> node = eat_node()) {
      if (nodes.empty()) {
        nodes.push_back(start);
      }
      nodes.push_back(node);
    }
    if (nodes.empty()) {
      return start;
    } else {
      return std::make_shared<CompoundExpression>(start->get_start_pos(), start->get_end_pos(), nodes);
    }
  }

  /**
   * handle start node:
   *    literal,
   *    parenExpr,
   *    function,
   *    variable
   *    projection,
   *    selection,
   *    firstSelection,
   *    lastSelection,
   *    indexer
   * @return
   */
  std::shared_ptr<AstNode> eat_start_node() {
    if (maybe_eat_literal()) {
      return pop_node();
    }
    if (maybe_eat_paren_expression()) {
      return pop_node();
    }
    if (maybe_eat_method_or_property(false)) {
      return pop_node();
    }
    if (maybe_eat_projection(false)) {
      return pop_node();
    }
    if (maybe_eat_selection(false)) {
      return pop_node();
    }
    if (maybe_eat_indexer()) {
      return pop_node();
    }
    if (maybe_eat_inline_list_or_map()) {
      return pop_node();
    }
    return nullptr;
  }

  /**
   * handle literal:
   *    LITERAL_BOOL,
   *    LITERAL_INT,
   *    LITERAL_FLOAT,
   *    LITERAL_STRING
   * @return
   */
  bool maybe_eat_literal() {
    Token token = peek_token();
    if (token.kind_ == Token::Kind::LITERAL_BOOL) {
      bool value = "true" == expr_str_.substr(token.start_pos_, token.end_pos_ - token.start_pos_);
      push_node(std::make_shared<LiteralBool>(token.start_pos_, token.end_pos_, value));
    } else if (token.kind_ == Token::Kind::LITERAL_INT) {
      int value = 0;
      for (int pos = token.start_pos_; pos < token.end_pos_; ++pos) {
        value = value * 10 + (expr_str_[pos] - '0');
      }
      push_node(std::make_shared<LiteralInt>(token.start_pos_, token.end_pos_, value));
    } else if (token.kind_ == Token::Kind::LITERAL_FLOAT) {
      float value = 0;
      bool dot = false;
      int rate = 1;
      for (int pos = token.start_pos_; pos < token.end_pos_; ++pos) {
        if (expr_str_[pos] == '.') {
          dot = true;
          continue;
        }
        if (dot) {
          rate *= 10;
          value += (expr_str_[pos] - '0') / ((float) rate);
        } else {
          value = value * 10;
        }
      }
      push_node(std::make_shared<LiteralFloat>(token.start_pos_, token.end_pos_, value));
    } else if (token.kind_ == Token::Kind::LITERAL_STRING) {
      push_node(std::make_shared<LiteralString>(token.start_pos_,
                                                token.end_pos_,
                                                expr_str_.substr(token.start_pos_ + 1,
                                                                 token.end_pos_ - token.start_pos_ - 1)));
    } else {
      return false;
    }
    next_token();
    return true;
  }

  bool maybe_eat_paren_expression() {
    if (peek_token().kind_ != Token::Kind::LPAREN) {
      return false;
    }

    Token token = next_token();
    std::shared_ptr<AstNode> expr = eat_expression();
    if (!expr) {
      CPPEL_THROW(ParseError("unexpected null after " + token.start_pos_));
    }
    eat_token(Token::Kind::RPAREN);
    push_node(expr);
    return true;
  }

  bool maybe_eat_method_or_property(bool save_navi) {
    if (peek_token().kind_ != Token::Kind::IDENTIFIER) {
      return false;
    }

    Token token = next_token();
    std::string name = expr_str_.substr(token.start_pos_, token.end_pos_ - token.start_pos_);
    std::vector<std::shared_ptr<AstNode>> args;
    if (maybe_eat_method_args(args)) {
      push_node(std::make_shared<FunctionNode>(token.start_pos_, token.end_pos_, save_navi, name, args));
    } else {
      push_node(std::make_shared<PropertyNode>(token.start_pos_, token.end_pos_, save_navi, name));
    }
    return true;
  }

  bool maybe_eat_method_args(std::vector<std::shared_ptr<AstNode>> &args) {
    if (peek_token().kind_ != Token::Kind::LPAREN) {
      return false;
    }
    consume_arguments(args);
    eat_token(Token::Kind::RPAREN);
    return true;
  }

  void consume_arguments(std::vector<std::shared_ptr<AstNode>> &args) {
    do {
      next_token();
      Token token = peek_token();
      if (token.kind_ == Token::Kind::END) {
        CPPEL_THROW(ParseError("unexpected end at" + token.start_pos_));
      }
      if (token.kind_ != Token::Kind::RPAREN) {
        args.push_back(eat_expression());
      }
    } while (peek_token().kind_ == Token::Kind::COMMA);
  }

  bool maybe_eat_projection(bool safe_navi) {
    if (peek_token().kind_ != Token::Kind::PROJECT) {
      return false;
    }

    Token token = next_token();
    std::shared_ptr<AstNode> expr = eat_expression();
    if (!expr) {
      CPPEL_THROW(ParseError("unexpected null after " + token.end_pos_));
    }
    eat_token(Token::Kind::RSQUARE);
    push_node(std::make_shared<Projection>(token.start_pos_, token.end_pos_, safe_navi, expr));
    return true;
  }

  bool maybe_eat_selection(bool safe_navi) {
    Token &pt = peek_token();
    if (pt.kind_ != Token::Kind::SELECT_FIRST && pt.kind_ != Token::Kind::SELECT_LAST
        && pt.kind_ != Token::Kind::SELECT) {
      return false;
    }

    Token token = next_token();
    std::shared_ptr<AstNode> expr = eat_expression();
    if (!expr) {
      CPPEL_THROW(ParseError("unexpected null after " + token.end_pos_));
    }
    eat_token(Token::Kind::RSQUARE);
    if (token.kind_ == Token::Kind::SELECT_FIRST) {
      push_node(std::make_shared<Selection>(token.start_pos_,
                                            token.end_pos_,
                                            safe_navi,
                                            Selection::SelectType::FIRST,
                                            expr));
    } else if (pt.kind_ == Token::Kind::SELECT_LAST) {
      push_node(std::make_shared<Selection>(token.start_pos_,
                                            token.end_pos_,
                                            safe_navi,
                                            Selection::SelectType::LAST,
                                            expr));
    } else {
      push_node(std::make_shared<Selection>(token.start_pos_,
                                            token.end_pos_,
                                            safe_navi,
                                            Selection::SelectType::ALL,
                                            expr));
    }
    return true;
  }

  bool maybe_eat_indexer() {
    if (peek_token().kind_ != Token::Kind::LSQUARE) {
      return false;
    }

    Token token = next_token();
    std::shared_ptr<AstNode> expr = eat_expression();
    if (!expr) {
      CPPEL_THROW(ParseError("unexpected null after " + token.end_pos_));
    }
    eat_token(Token::Kind::RSQUARE);
    push_node(std::make_shared<Indexer>(token.start_pos_, token.end_pos_, expr));
    return true;
  }

  /**
   * list: LCURLY (element (COMMA element)*) RCURLY
   * map:  LCURLY (key ':' value (COMMA key ':' value)*) RCURLY
   * @return
   */
  bool maybe_eat_inline_list_or_map() {
    if (peek_token().kind_ != Token::Kind::LCURLY) {
      return false;
    }
    Token token = next_token();
    Token &pt = peek_token();
    if (pt.kind_ == Token::Kind::RCURLY) {
      push_node(std::make_shared<InlineList>(token.start_pos_, pt.end_pos_));
    } else if (pt.kind_ == Token::Kind::COLON) {
      next_token();
      Token close_token = eat_token(Token::Kind::RCURLY);
      push_node(std::make_shared<InlineMap>(token.start_pos_, close_token.end_pos_));
    } else {
      std::vector<std::shared_ptr<AstNode>> exprs;
      exprs.push_back(eat_expression());
      if (peek_token().kind_ == Token::Kind::RCURLY) {
        Token close_token = eat_token(Token::Kind::RCURLY);
        push_node(std::make_shared<InlineList>(token.start_pos_, close_token.end_pos_, exprs));
      } else if (peek_token().kind_ == Token::Kind::COMMA) {
        do {
          next_token();
          exprs.push_back(eat_expression());
        } while (peek_token().kind_ == Token::Kind::COMMA);
        Token close_token = eat_token(Token::Kind::RCURLY);
        push_node(std::make_shared<InlineList>(token.start_pos_, close_token.end_pos_, exprs));
      } else if (peek_token().kind_ == Token::Kind::COLON) {
        next_token();
        exprs.push_back(eat_expression());
        while (peek_token().kind_ == Token::Kind::COMMA) {
          next_token();
          exprs.push_back(eat_expression());
          eat_token(Token::Kind::COLON);
          exprs.push_back(eat_expression());
        }
        Token close_token = eat_token(Token::Kind::RCURLY);
        push_node(std::make_shared<InlineMap>(token.start_pos_, close_token.end_pos_, exprs));
      } else {
        CPPEL_THROW(ParseError("unexpected token after " + peek_token().start_pos_));
      }
    }
    return true;
  }

  /**
   * handle node : ((DOT dottedNode) | (SAFE_NAVI dottedNode) | nonDottedNode)+;
   * @return
   */
  std::shared_ptr<AstNode> eat_node() {
    Token &pt = peek_token();
    if (pt.kind_ == Token::Kind::DOT || pt.kind_ == Token::Kind::SAFE_NAVI) {
      return eat_dotted_node();
    } else {
      return eat_non_dotted_node();
    }
  }

  /**
   * handle:
   *    method,
   *    property,
   *    projection,
   *    selection
   * @return
   */
  std::shared_ptr<AstNode> eat_dotted_node() {
    Token token = next_token();
    bool safe_navi = token.kind_ == Token::Kind::SAFE_NAVI;
    if (maybe_eat_method_or_property(safe_navi) || maybe_eat_projection(safe_navi) || maybe_eat_selection(safe_navi)) {
      return pop_node();
    }
    CPPEL_THROW(ParseError("unexpected token after " + peek_token().start_pos_));
  }

  /**
   * handle: IDENTIFIER[EXPRESSION]
   * @return
   */
  std::shared_ptr<AstNode> eat_non_dotted_node() {
    if (peek_token().kind_ == Token::Kind::LSQUARE) {
      if (maybe_eat_indexer()) {
        return pop_node();
      }
    }
    return nullptr;
  }

  Token &peek_token() {
    return tokenizer_.peek_token();
  }

  Token next_token() {
    return tokenizer_.next_token();
  }

  bool peek_token(const Token::Kind expected_kind) {
    Token &token = peek_token();
    return token.kind_ == expected_kind;
  }

  Token eat_token(const Token::Kind expected_kind) {
    Token token = next_token();
    if (token.kind_ != expected_kind) {
      CPPEL_THROW(ParseError("expect can't be match at " + token.start_pos_));
    }
    return token;
  }

  void push_node(const std::shared_ptr<AstNode> node) {
    constructedNodes_.push_back(node);
  }

  std::shared_ptr<AstNode> pop_node() {
    std::shared_ptr<AstNode> node = constructedNodes_.back();
    constructedNodes_.pop_back();
    return node;
  }

};

class Parser {
 public:
  Parser() {}
  Expression parse(const std::string &expr_str) {
    if (expr_str.empty()) {
      CPPEL_THROW(ParseError("unexpected empty string"));
    }
    InternalParser internal_parser(expr_str);
    std::shared_ptr<AstNode> root = internal_parser.parse();
    if (!root) {
      CPPEL_THROW(ParseError("internal parser error"));
    }
    return Expression(root);
  }
};

} // namespace cppel
