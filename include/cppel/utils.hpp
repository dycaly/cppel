//
// Created by dycaly on 22-10-3.
//

#pragma once

#include <memory>
#include "nlohmann/json.hpp"

namespace cppel {
using json = nlohmann::json;

static bool truthy(const json *data) {
  if (!data) {
    return false;
  } else if (data->is_boolean()) {
    return data->get<bool>();
  } else if (data->is_number()) {
    return (*data != 0);
  } else if (data->is_null()) {
    return false;
  }
  return !data->empty();
}

} // namespace cppel