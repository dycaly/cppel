#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include "ast.hpp"
#include "context.hpp"

namespace cppel {

using json = nlohmann::json;

class Expression {
 public:
  Expression(const std::shared_ptr<AstNode> root) : root_(root) {}

  std::shared_ptr<json> evaluate(const json &data) {
    EvaluationContext context = EvaluationContext(data);
    return evaluate(context);
  }

  std::shared_ptr<json> evaluate(EvaluationContext &context) {
    return root_->evaluate(context);
  }

 private:
  std::shared_ptr<AstNode> root_;
};

}  // namespace cppel