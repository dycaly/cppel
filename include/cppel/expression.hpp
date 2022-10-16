//
// Created by dycaly on 22-10-3.
//

#pragma once

#include <memory>
#include "nlohmann/json.hpp"
#include "ast.hpp"
#include "context.hpp"

namespace cppel {

using json = nlohmann::json;

class Expression {
 public:
  Expression(const std::shared_ptr<AstNode> root) : root_(root) {}

  json evaluate(const json &data) {
    EvaluationContext context = EvaluationContext(data);
    json rlt = *root_->evaluate(context);
    context.clear_ref();
    return rlt;
  }

  json evaluate(EvaluationContext &context) {
    json rlt = *root_->evaluate(context);
    context.clear_ref();
    return rlt;
  }

 private:
  std::shared_ptr<AstNode> root_;
};

}  // namespace cppel