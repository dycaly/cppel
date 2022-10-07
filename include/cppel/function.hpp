//
// Created by dycaly on 22-10-3.
//

#pragma once

#include <vector>
#include <sstream>
#include "nlohmann/json.hpp"

namespace cppel {

using json = nlohmann::json;

class PresetFunction {
 public:
  static std::shared_ptr<json> join(std::vector<std::shared_ptr<json>> &args) {
    auto list = args[0];
    std::string joiner = args[1]->get<std::string>();
    std::stringstream ss;
    for (auto it = list->begin(); it != list->end(); ++it) {
      std::string str = it->get<std::string>();
      if (it != list->begin()) {
        ss << joiner;
      }
      ss << str;
    }
    return std::make_shared<json>(ss.str());
  }

  static std::shared_ptr<json> split(std::vector<std::shared_ptr<json>> &args) {
    std::string source_str = args[0]->get<std::string>();
    std::string splitter = args[1]->get<std::string>();
    auto result = std::make_shared<json>();
    int src_l = 0, src_r = 0;
    while (src_r < source_str.size()) {
      int spl_i = 0;
      while (src_r + spl_i < source_str.size() && spl_i < splitter.size() && source_str[src_r] == splitter[spl_i])
        ++spl_i;
      if (spl_i == splitter.size()) {
        result->push_back(source_str.substr(src_l, src_r - src_l));
        src_r += spl_i;
        src_l = src_r;
      } else {
        ++src_r;
      }
    }
    result->push_back(source_str.substr(src_l, src_r - src_l));
    return result;
  }
};

} // namespace cppel