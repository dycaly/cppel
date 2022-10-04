#pragma once

#include <string>
#include <deque>
#include <map>
#include <nlohmann/json.hpp>
#include "function.hpp"
#include "exception.hpp"

namespace cppel {
using json = nlohmann::json;

using Arguments = std::vector<std::shared_ptr<json>>;
using Function = std::function<std::shared_ptr<json>(Arguments &args)>;

class EvaluationContext {
 public:
  EvaluationContext(json &root_context) : root_context_(&root_context) {}

  json *get_active_context() {
    if (context_deque_.empty()) {
      return root_context_;
    }
    return context_deque_.back();
  }

  void push_context(json *context) {
    context_deque_.push_back(context);
  }

  json *pop_context() {
    json *rlt = context_deque_.back();
    context_deque_.pop_back();
    return rlt;
  }

  void add_function(const std::pair<std::string, int> &name_args_count, const Function function) {
    functions_[name_args_count] = function;
  }

  Function &get_function(const std::pair<std::string, int> &name_args_count) {
    if (functions_.find(name_args_count) != functions_.end()) {
      return functions_[name_args_count];
    }
    CPPEL_THROW(EvaluateError("function [" + name_args_count.first + "] with args_count " + std::to_string(name_args_count.second) +" not exits"));
  }

 private:
  json *root_context_;
  std::deque<json *> context_deque_;

  std::map<std::pair<std::string, int>, Function> functions_ = {
      {std::make_pair("join", 2), PresetFunction::join},
      {std::make_pair("split", 2), PresetFunction::split}
  };
};

} // namespace cppel