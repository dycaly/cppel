#pragma once

#include "ast.hpp"

namespace cppel {

class Expression {
 public:
  Expression(const std::shared_ptr<AstNode> root) : root_(root) {}
 private:
  std::shared_ptr<AstNode> root_;
};

}  // namespace cppel