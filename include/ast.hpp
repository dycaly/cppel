#pragma once

#include <memory>
#include <string>
#include <vector>

namespace cppel {

class AstNode {
 public:
  AstNode(const size_t start_pos, const size_t end_pos)
      : start_pos_(start_pos), end_pos_(end_pos) {}
  virtual ~AstNode() {}

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
};

class LiteralBool : public AstNode {
 public:
  LiteralBool(const size_t start_pos, const size_t end_pos, const bool value)
      : AstNode(start_pos, end_pos), value_(value) {}

 private:
  bool value_;
};

class LiteralInt : public AstNode {
 public:
  LiteralInt(const size_t start_pos, const size_t end_pos, const int value)
      : AstNode(start_pos, end_pos), value_(value) {}

 private:
  int value_;
};

class LiteralFloat : public AstNode {
 public:
  LiteralFloat(const size_t start_pos, const size_t end_pos, const float value)
      : AstNode(start_pos, end_pos), value_(value) {}

 private:
  float value_;
};

class LiteralString : public AstNode {
 public:
  LiteralString(const size_t start_pos, const size_t end_pos, const std::string &value)
      : AstNode(start_pos, end_pos), value_(value) {}

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
 private:
  std::string function_name_;
  std::vector<std::shared_ptr<AstNode>> exprs_;
};

class PropertyNode : public AstNode {
 public:
  PropertyNode(const size_t start_pos,
               const size_t end_pos,
               const bool null_safe,
               const std::string &property_name) :
      AstNode(start_pos, end_pos), null_safe_(null_safe), property_name_(property_name) {}
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
 private:
  std::vector<std::shared_ptr<AstNode>> exprs_;
};

class CompoundExpression : public AstNode {
 public:
  CompoundExpression(const size_t start_pos,
                     const size_t end_pos,
                     const std::vector<std::shared_ptr<AstNode>> exprs) :
      AstNode(start_pos, end_pos), exprs_(exprs) {}
 private:
  std::vector<std::shared_ptr<AstNode>> exprs_;
};

}  // namespace cppel
