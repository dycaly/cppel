//
// Created by dycaly on 22-10-3.
//

#pragma once

#include <string>
#include <deque>
#include <map>
#include "nlohmann/json.hpp"
#include "function.hpp"
#include "exception.hpp"

namespace cppel {
using json = nlohmann::json;

using Arguments = std::vector<std::shared_ptr<json>>;
using Function = std::function<std::shared_ptr<json>(Arguments &args)>;

class EvaluationContext {
 public:
  EvaluationContext(const json &root_data) : root_data_(&root_data) {}

  const json *get_active_data() {
    if (data_deque_.empty()) {
      return root_data_;
    }
    return data_deque_.back();
  }

  void push_data(const json *data) {
    data_deque_.push_back(data);
  }

  const json *pop_data() {
    const json *data = data_deque_.back();
    data_deque_.pop_back();
    return data;
  }

  void add_function(const std::pair<std::string, int> &name_args_count, const Function function) {
    functions_[name_args_count] = function;
  }

  Function &get_function(const std::pair<std::string, int> &name_args_count) {
    if (functions_.find(name_args_count) != functions_.end()) {
      return functions_[name_args_count];
    }
    CPPEL_THROW(EvaluateError(
        "function [" + name_args_count.first + "] with args_count " + std::to_string(name_args_count.second)
            + " not exits"));
  }

 private:
  const json *root_data_;
  std::deque<const json *> data_deque_;

  std::map<std::pair<std::string, int>, Function> functions_ = {
      {std::make_pair("join", 2), PresetFunction::join},
      {std::make_pair("split", 2), PresetFunction::split}
  };
};

} // namespace cppel