//
// Created by dycaly on 22-10-3.
//

#pragma once

#include <memory>
#include <string>
#include <vector>
#include "nlohmann/json.hpp"
#include "context.hpp"
#include "exception.hpp"
#include "utils.hpp"

namespace cppel {

using json = nlohmann::json;

class AstNode {
 public:
  AstNode(const size_t start_pos, const size_t end_pos)
      : start_pos_(start_pos), end_pos_(end_pos) {}
  virtual ~AstNode() {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {
    return nullptr;
  }

  size_t get_start_pos() {
    return start_pos_;
  }

  size_t get_end_pos() {
    return end_pos_;
  }

 private:
  size_t start_pos_;
  size_t end_pos_;
  std::vector<std::shared_ptr<AstNode>> children_;
};

class LiteralNone : public AstNode {
 public:
  LiteralNone(const size_t start_pos, const size_t end_pos)
      : AstNode(start_pos, end_pos) {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {
    return nullptr;
  }
};

class LiteralBool : public AstNode {
 public:
  LiteralBool(const size_t start_pos, const size_t end_pos, const bool value)
      : AstNode(start_pos, end_pos), value_(value) {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {
    return std::make_shared<json>(value_);
  }

 private:
  bool value_;
};

class LiteralInt : public AstNode {
 public:
  LiteralInt(const size_t start_pos, const size_t end_pos, const int value)
      : AstNode(start_pos, end_pos), value_(value) {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {
    return std::make_shared<json>(value_);
  }

 private:
  int value_;
};

class LiteralFloat : public AstNode {
 public:
  LiteralFloat(const size_t start_pos, const size_t end_pos, const float value)
      : AstNode(start_pos, end_pos), value_(value) {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {
    return std::make_shared<json>(value_);
  }

 private:
  float value_;
};

class LiteralString : public AstNode {
 public:
  LiteralString(const size_t start_pos, const size_t end_pos, const std::string &value)
      : AstNode(start_pos, end_pos), value_(value) {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {
    return std::make_shared<json>(value_);
  }

 private:
  std::string value_;
};

class Assign : public AstNode {
 public:
  Assign(const size_t start_pos,
         const size_t end_pos,
         const std::shared_ptr<AstNode> assignee,
         const std::shared_ptr<AstNode> assigned_value) :
      AstNode(start_pos, end_pos), assignee_(assignee), assigned_value_(assigned_value) {}

 private:
  std::shared_ptr<AstNode> assignee_;
  std::shared_ptr<AstNode> assigned_value_;
};

class Elvis : public AstNode {
 public:
  Elvis(const size_t start_pos,
        const size_t end_pos,
        const std::shared_ptr<AstNode> if_value,
        const std::shared_ptr<AstNode> else_value) :
      AstNode(start_pos, end_pos), if_value_(if_value), else_value_(else_value) {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {
    std::shared_ptr<json> value = if_value_->evaluate(context);
    if (!value) {
      value = else_value_->evaluate(context);
    }
    return value;
  }

 private:
  std::shared_ptr<AstNode> if_value_;
  std::shared_ptr<AstNode> else_value_;
};

class Ternary : public AstNode {
 public:
  Ternary(const size_t start_pos,
          const size_t end_pos,
          const std::shared_ptr<AstNode> condition,
          const std::shared_ptr<AstNode> if_true_value,
          const std::shared_ptr<AstNode> if_false_value) :
      AstNode(start_pos, end_pos),
      condition_(condition),
      if_true_value_(if_true_value),
      if_false_value_(if_false_value) {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {
    return truthy(condition_->evaluate(context)) ?
           if_true_value_->evaluate(context) :
           if_false_value_->evaluate(context);
  }

 private:
  std::shared_ptr<AstNode> condition_;
  std::shared_ptr<AstNode> if_true_value_;
  std::shared_ptr<AstNode> if_false_value_;
};

class OpOr : public AstNode {
 public:
  OpOr(const size_t start_pos,
       const size_t end_pos,
       const std::shared_ptr<AstNode> lh_expr,
       const std::shared_ptr<AstNode> rh_expr) :
      AstNode(start_pos, end_pos), lh_expr_(lh_expr), rh_expr_(rh_expr) {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {
    bool value = truthy(lh_expr_->evaluate(context)) || truthy(rh_expr_->evaluate(context));
    return std::make_shared<json>(value);
  }

 private:
  std::shared_ptr<AstNode> lh_expr_;
  std::shared_ptr<AstNode> rh_expr_;
};

class OpAnd : public AstNode {
 public:
  OpAnd(const size_t start_pos,
        const size_t end_pos,
        const std::shared_ptr<AstNode> lh_expr,
        const std::shared_ptr<AstNode> rh_expr) :
      AstNode(start_pos, end_pos), lh_expr_(lh_expr), rh_expr_(rh_expr) {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {
    bool value = truthy(lh_expr_->evaluate(context)) && truthy(rh_expr_->evaluate(context));
    return std::make_shared<json>(value);
  }

 private:
  std::shared_ptr<AstNode> lh_expr_;
  std::shared_ptr<AstNode> rh_expr_;
};

class OpGT : public AstNode {
 public:
  OpGT(const size_t start_pos,
       const size_t end_pos,
       const std::shared_ptr<AstNode> lh_expr,
       const std::shared_ptr<AstNode> rh_expr) :
      AstNode(start_pos, end_pos), lh_expr_(lh_expr), rh_expr_(rh_expr) {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {
    bool value = *(lh_expr_->evaluate(context).get()) > *(rh_expr_->evaluate(context).get());
    return std::make_shared<json>(value);
  }

 private:
  std::shared_ptr<AstNode> lh_expr_;
  std::shared_ptr<AstNode> rh_expr_;
};

class OpGE : public AstNode {
 public:
  OpGE(const size_t start_pos,
       const size_t end_pos,
       const std::shared_ptr<AstNode> lh_expr,
       const std::shared_ptr<AstNode> rh_expr) :
      AstNode(start_pos, end_pos), lh_expr_(lh_expr), rh_expr_(rh_expr) {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {
    bool value = *(lh_expr_->evaluate(context).get()) >= *(rh_expr_->evaluate(context).get());
    return std::make_shared<json>(value);
  }

 private:
  std::shared_ptr<AstNode> lh_expr_;
  std::shared_ptr<AstNode> rh_expr_;
};

class OpLT : public AstNode {
 public:
  OpLT(const size_t start_pos,
       const size_t end_pos,
       const std::shared_ptr<AstNode> lh_expr,
       const std::shared_ptr<AstNode> rh_expr) :
      AstNode(start_pos, end_pos), lh_expr_(lh_expr), rh_expr_(rh_expr) {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {
    bool value = *(lh_expr_->evaluate(context).get()) < *(rh_expr_->evaluate(context).get());
    return std::make_shared<json>(value);
  }

 private:
  std::shared_ptr<AstNode> lh_expr_;
  std::shared_ptr<AstNode> rh_expr_;
};

class OpLE : public AstNode {
 public:
  OpLE(const size_t start_pos,
       const size_t end_pos,
       const std::shared_ptr<AstNode> lh_expr,
       const std::shared_ptr<AstNode> rh_expr) :
      AstNode(start_pos, end_pos), lh_expr_(lh_expr), rh_expr_(rh_expr) {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {
    bool value = *(lh_expr_->evaluate(context).get()) <= *(rh_expr_->evaluate(context).get());
    return std::make_shared<json>(value);
  }

 private:
  std::shared_ptr<AstNode> lh_expr_;
  std::shared_ptr<AstNode> rh_expr_;
};

class OpEQ : public AstNode {
 public:
  OpEQ(const size_t start_pos,
       const size_t end_pos,
       const std::shared_ptr<AstNode> lh_expr,
       const std::shared_ptr<AstNode> rh_expr) :
      AstNode(start_pos, end_pos), lh_expr_(lh_expr), rh_expr_(rh_expr) {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {
    bool value = *(lh_expr_->evaluate(context).get()) == *(rh_expr_->evaluate(context).get());
    return std::make_shared<json>(value);
  }

 private:
  std::shared_ptr<AstNode> lh_expr_;
  std::shared_ptr<AstNode> rh_expr_;
};

class OpNE : public AstNode {
 public:
  OpNE(const size_t start_pos,
       const size_t end_pos,
       const std::shared_ptr<AstNode> lh_expr,
       const std::shared_ptr<AstNode> rh_expr) :
      AstNode(start_pos, end_pos), lh_expr_(lh_expr), rh_expr_(rh_expr) {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {
    bool value = *(lh_expr_->evaluate(context).get()) != *(rh_expr_->evaluate(context).get());
    return std::make_shared<json>(value);
  }

 private:
  std::shared_ptr<AstNode> lh_expr_;
  std::shared_ptr<AstNode> rh_expr_;
};

class OpPlus : public AstNode {
 public:
  OpPlus(const size_t start_pos,
         const size_t end_pos,
         const std::shared_ptr<AstNode> lh_expr,
         const std::shared_ptr<AstNode> rh_expr) :
      AstNode(start_pos, end_pos), lh_expr_(lh_expr), rh_expr_(rh_expr) {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {
    std::shared_ptr<json> lh_value = lh_expr_ ? lh_expr_->evaluate(context) : std::make_shared<json>(0);
    std::shared_ptr<json> rh_value = rh_expr_ ? rh_expr_->evaluate(context) : std::make_shared<json>(0);
    if (lh_value->is_string() && rh_value->is_string()) {
      return std::make_shared<json>(lh_value->get<std::string>() + rh_value->get<std::string>());
    } else if (lh_value->is_number_integer() && rh_value->is_number_integer()) {
      return std::make_shared<json>(lh_value->get<int>() + rh_value->get<int>());
    } else {
      return std::make_shared<json>(lh_value->get<float>() + rh_value->get<float>());
    }
  }

 private:
  std::shared_ptr<AstNode> lh_expr_;
  std::shared_ptr<AstNode> rh_expr_;
};

class OpMinus : public AstNode {
 public:
  OpMinus(const size_t start_pos,
          const size_t end_pos,
          const std::shared_ptr<AstNode> lh_expr,
          const std::shared_ptr<AstNode> rh_expr) :
      AstNode(start_pos, end_pos), lh_expr_(lh_expr), rh_expr_(rh_expr) {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {
    std::shared_ptr<json> lh_value = lh_expr_ ? lh_expr_->evaluate(context) : std::make_shared<json>(0);
    std::shared_ptr<json> rh_value = rh_expr_ ? rh_expr_->evaluate(context) : std::make_shared<json>(0);
    if (lh_value->is_number_integer() && rh_value->is_number_integer()) {
      return std::make_shared<json>(lh_value->get<int>() - rh_value->get<int>());
    } else {
      return std::make_shared<json>(lh_value->get<float>() - rh_value->get<float>());
    }
  }

 private:
  std::shared_ptr<AstNode> lh_expr_;
  std::shared_ptr<AstNode> rh_expr_;
};

class OpMultiply : public AstNode {
 public:
  OpMultiply(const size_t start_pos,
             const size_t end_pos,
             const std::shared_ptr<AstNode> lh_expr,
             const std::shared_ptr<AstNode> rh_expr) :
      AstNode(start_pos, end_pos), lh_expr_(lh_expr), rh_expr_(rh_expr) {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {
    std::shared_ptr<json> lh_value = lh_expr_->evaluate(context);
    std::shared_ptr<json> rh_value = rh_expr_->evaluate(context);
    if (lh_value->is_number_integer() && rh_value->is_number_integer()) {
      return std::make_shared<json>(lh_value->get<int>() * rh_value->get<int>());
    } else {
      return std::make_shared<json>(lh_value->get<float>() * rh_value->get<float>());
    }
  }

 private:
  std::shared_ptr<AstNode> lh_expr_;
  std::shared_ptr<AstNode> rh_expr_;
};

class OpDivide : public AstNode {
 public:
  OpDivide(const size_t start_pos,
           const size_t end_pos,
           const std::shared_ptr<AstNode> lh_expr,
           const std::shared_ptr<AstNode> rh_expr) :
      AstNode(start_pos, end_pos), lh_expr_(lh_expr), rh_expr_(rh_expr) {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {
    std::shared_ptr<json> lh_value = lh_expr_->evaluate(context);
    std::shared_ptr<json> rh_value = rh_expr_->evaluate(context);
    if (lh_value->is_number_integer() && rh_value->is_number_integer()) {
      return std::make_shared<json>(lh_value->get<int>() / rh_value->get<int>());
    } else {
      return std::make_shared<json>(lh_value->get<float>() / rh_value->get<float>());
    }
  }

 private:
  std::shared_ptr<AstNode> lh_expr_;
  std::shared_ptr<AstNode> rh_expr_;
};

class OpModulus : public AstNode {
 public:
  OpModulus(const size_t start_pos,
            const size_t end_pos,
            const std::shared_ptr<AstNode> lh_expr,
            const std::shared_ptr<AstNode> rh_expr) :
      AstNode(start_pos, end_pos), lh_expr_(lh_expr), rh_expr_(rh_expr) {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {
    std::shared_ptr<json> lh_value = lh_expr_->evaluate(context);
    std::shared_ptr<json> rh_value = rh_expr_->evaluate(context);
    return std::make_shared<json>(lh_value->get<int>() % rh_value->get<int>());
  }

 private:
  std::shared_ptr<AstNode> lh_expr_;
  std::shared_ptr<AstNode> rh_expr_;
};

class OpPower : public AstNode {
 public:
  OpPower(const size_t start_pos,
          const size_t end_pos,
          const std::shared_ptr<AstNode> lh_expr,
          const std::shared_ptr<AstNode> rh_expr) :
      AstNode(start_pos, end_pos), lh_expr_(lh_expr), rh_expr_(rh_expr) {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {
    std::shared_ptr<json> lh_value = lh_expr_->evaluate(context);
    std::shared_ptr<json> rh_value = rh_expr_->evaluate(context);
    if (lh_value->is_number_integer() && rh_value->is_number_integer() && rh_value->get<int>() > 0) {
      return std::make_shared<json>(static_cast<int>(std::pow(lh_value->get<int>(), rh_value->get<int>())));
    } else {
      return std::make_shared<json>(std::pow(lh_value->get<float>(), rh_value->get<float>()));
    }
  }

 private:
  std::shared_ptr<AstNode> lh_expr_;
  std::shared_ptr<AstNode> rh_expr_;
};

class OpNot : public AstNode {
 public:
  OpNot(const size_t start_pos,
        const size_t end_pos,
        const std::shared_ptr<AstNode> expr) :
      AstNode(start_pos, end_pos), expr_(expr) {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {
    return std::make_shared<json>(!truthy(expr_->evaluate(context)));
  }

 private:
  std::shared_ptr<AstNode> expr_;
};

class FunctionNode : public AstNode {
 public:
  FunctionNode(const size_t start_pos,
               const size_t end_pos,
               const std::string &function_name,
               const std::vector<std::shared_ptr<AstNode>> exprs) :
      AstNode(start_pos, end_pos), function_name_(function_name), exprs_(exprs) {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {

    Function function = context.get_function(std::make_pair(function_name_, exprs_.size()));

    std::vector<std::shared_ptr<json>> args;
    for (auto expr : exprs_) {
      args.push_back(expr->evaluate(context));
    }
    return function(args);
  }

 private:
  std::string function_name_;
  std::vector<std::shared_ptr<AstNode>> exprs_;
};

class VariableNode : public AstNode {
 public:
  VariableNode(const size_t start_pos,
               const size_t end_pos,
               const std::string &variable_name) :
      AstNode(start_pos, end_pos), variable_name_(variable_name) {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {
    const json *root = context.get_active_data();
    if (variable_name_ == "this") {
      return std::make_shared<json>(*root);
    }
    CPPEL_THROW(EvaluateError("unexpected variable at" + std::to_string(get_start_pos())));
  }

 private:
  std::string variable_name_;
};

class MethodNode : public AstNode {
 public:
  MethodNode(const size_t start_pos,
             const size_t end_pos,
             const bool null_safe,
             const std::string &method_name,
             const std::vector<std::shared_ptr<AstNode>> exprs) :
      AstNode(start_pos, end_pos), null_safe_(null_safe), method_name_(method_name), exprs_(exprs) {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {
    std::vector<std::shared_ptr<json>> args;
    for (auto expr : exprs_) {
      args.push_back(expr->evaluate(context));
    }
    // TODO: Method call

    return std::make_shared<json>();
  }

 private:
  bool null_safe_;
  std::string method_name_;
  std::vector<std::shared_ptr<AstNode>> exprs_;
};

class PropertyNode : public AstNode {
 public:
  PropertyNode(const size_t start_pos,
               const size_t end_pos,
               const bool null_safe,
               const std::string &property_name) :
      AstNode(start_pos, end_pos), null_safe_(null_safe), property_name_(property_name) {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {
    const json *root = context.get_active_data();
    if (root == nullptr) {
      if (null_safe_) {
        return nullptr;
      } else {
        CPPEL_THROW(EvaluateError("unexpected null at" + std::to_string(get_start_pos())));
      }
    }

    if (root->contains(property_name_)) {
      return std::make_shared<json>(root->at(property_name_));
    } else {
      return nullptr;
    }
  }

 private:
  bool null_safe_;
  std::string property_name_;
};

class Projection : public AstNode {
 public:
  Projection(const size_t start_pos,
             const size_t end_pos,
             const bool null_safe,
             const std::shared_ptr<AstNode> expr) :
      AstNode(start_pos, end_pos), null_safe_(null_safe), expr_(expr) {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {
    const json *root = context.get_active_data();
    if (root == nullptr) {
      if (null_safe_) {
        return nullptr;
      } else {
        CPPEL_THROW(EvaluateError("unexpected null at" + std::to_string(get_start_pos())));
      }
    }

    std::shared_ptr<json> result = std::make_shared<json>();
    for (auto it = root->begin(); it != root->end(); ++it) {
      context.push_data(&(*it));
      result->push_back(*(expr_->evaluate(context)));
      context.pop_data();
    }
    return result;
  }

 private:
  bool null_safe_;
  std::shared_ptr<AstNode> expr_;
};

class Selection : public AstNode {
 public:
  enum class SelectType {
    FIRST,
    LAST,
    ALL
  };
  Selection(const size_t start_pos,
            const size_t end_pos,
            const bool null_safe,
            const SelectType type,
            const std::shared_ptr<AstNode> expr) :
      AstNode(start_pos, end_pos), null_safe_(null_safe), type_(type), expr_(expr) {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {
    const json *root = context.get_active_data();
    if (root == nullptr) {
      if (null_safe_) {
        return nullptr;
      } else {
        CPPEL_THROW(EvaluateError("unexpected null at" + std::to_string(get_start_pos())));
      }
    }

    if (type_ == SelectType::FIRST) {
      bool found = false;
      auto it = root->begin();
      while (it != root->end() && !found) {
        context.push_data(&(*it));
        if (!(found = truthy(expr_->evaluate(context)))) {
          ++it;
        }
        context.pop_data();
      }
      return found ? std::make_shared<json>(*it) : nullptr;
    } else if (type_ == SelectType::LAST) {
      bool found = false;
      auto it = root->rbegin();
      while (it != root->rend() && !found) {
        context.push_data(&(*it));
        if (!(found = truthy(expr_->evaluate(context)))) {
          ++it;
        }
        context.pop_data();
      }
      return found ? std::make_shared<json>(*it) : nullptr;
    } else {
      std::shared_ptr<json> result = std::make_shared<json>();
      for (auto it = root->begin(); it != root->end(); ++it) {
        context.push_data(&(*it));
        if (truthy(expr_->evaluate(context))) {
          result->push_back(*it);
        }
        context.pop_data();
      }
      return result;
    }
  }

 private:
  bool null_safe_;
  SelectType type_;
  std::shared_ptr<AstNode> expr_;
};

class Indexer : public AstNode {
 public:
  Indexer(const size_t start_pos,
          const size_t end_pos,
          const std::shared_ptr<AstNode> expr) :
      AstNode(start_pos, end_pos), expr_(expr) {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {
    const json *root = context.get_active_data();
    if (root == nullptr) {
      CPPEL_THROW(EvaluateError("unexpected null at" + std::to_string(get_start_pos())));
    }
    std::shared_ptr<json> index_value = expr_->evaluate(context);
    if (root->is_string()) {
      std::string str = root->get<std::string>();
      int index = index_value->get<int>();
      if (index >= root->size()) {
        CPPEL_THROW(EvaluateError("string out of index at" + std::to_string(get_start_pos())));
      }
      return std::make_shared<json>(str.substr(index, 1));
    } else if (root->is_array()) {
      int index = index_value->get<int>();
      if (index >= root->size()) {
        CPPEL_THROW(EvaluateError("array out of index at" + std::to_string(get_start_pos())));
      }
      return std::make_shared<json>(root->at(index));
    } else if (root->is_object()) {
      std::string key = index_value->get<std::string>();
      if (root->contains(key)) {
        return std::make_shared<json>(root->at(key));
      } else {
        CPPEL_THROW(EvaluateError("unexpected indexer at" + std::to_string(get_start_pos())));
      }
    }
    CPPEL_THROW(EvaluateError("can't be index at" + std::to_string(get_start_pos())));
  }

 private:
  std::shared_ptr<AstNode> expr_;
};

class InlineList : public AstNode {
 public:
  InlineList(const size_t start_pos,
             const size_t end_pos) :
      AstNode(start_pos, end_pos), exprs_() {}

  InlineList(const size_t start_pos,
             const size_t end_pos,
             const std::vector<std::shared_ptr<AstNode>> exprs) :
      AstNode(start_pos, end_pos), exprs_(exprs) {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {
    std::shared_ptr<json> array = std::make_shared<json>();
    for (auto expr : exprs_) {
      array->push_back(*(expr->evaluate(context).get()));
    }
    return array;
  }

 private:
  std::vector<std::shared_ptr<AstNode>> exprs_;
};

class InlineMap : public AstNode {
 public:
  InlineMap(const size_t start_pos,
            const size_t end_pos) :
      AstNode(start_pos, end_pos), exprs_() {}

  InlineMap(const size_t start_pos,
            const size_t end_pos,
            const std::vector<std::shared_ptr<AstNode>> exprs) :
      AstNode(start_pos, end_pos), exprs_(exprs) {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {
    std::shared_ptr<json> map = std::make_shared<json>();
    for (int i = 0; i < exprs_.size(); i += 2) {
      std::shared_ptr<json> key = exprs_[i]->evaluate(context);
      std::shared_ptr<json> value = exprs_[i + 1]->evaluate(context);
      if (key->is_number_integer()) {
        map->at(key->get<int>()) = *(value.get());
      } else {
        map->at(key->get<std::string>()) = *(value.get());
      }
    }
    return map;
  }

 private:
  std::vector<std::shared_ptr<AstNode>> exprs_;
};

class CompoundExpression : public AstNode {
 public:
  CompoundExpression(const size_t start_pos,
                     const size_t end_pos,
                     const std::vector<std::shared_ptr<AstNode>> exprs) :
      AstNode(start_pos, end_pos), exprs_(exprs) {}

  virtual std::shared_ptr<json> evaluate(EvaluationContext &context) {
    const json *root = context.get_active_data();
    if (root == nullptr) {
      CPPEL_THROW(EvaluateError("unexpected null at " + std::to_string(get_start_pos())));
    }

    std::shared_ptr<json> result = std::make_shared<json>(*root);
    for (auto expr : exprs_) {
      context.push_data(result.get());
      result = expr->evaluate(context);
      context.pop_data();
    }

    return result;
  }

 private:
  std::vector<std::shared_ptr<AstNode>> exprs_;
};

}  // namespace cppel
