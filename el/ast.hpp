#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace cppel {

using json = nlohmann::json;

class AstNode {
 public:
  AstNode(const size_t start_pos, const size_t end_pos)
      : start_pos_(start_pos), end_pos_(end_pos) {}

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
  json value_;
};

class LiteralInt : public AstNode {
 public:
  LiteralInt(const size_t start_pos, const size_t end_pos, const int value)
      : AstNode(start_pos, end_pos), value_(value) {}

 private:
  json value_;
};

class LiteralFloat : public AstNode {
 public:
  LiteralFloat(const size_t start_pos, const size_t end_pos, const float value)
      : AstNode(start_pos, end_pos), value_(value) {}

 private:
  json value_;
};

class LiteralString : public AstNode {
 public:
  LiteralString(const size_t start_pos, const size_t end_pos, const std::string &value)
      : AstNode(start_pos, end_pos), value_(value) {}

 private:
  json value_;
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

}  // namespace cppel
