#pragma once
#include <stdexcept>
#include <string>

namespace cppel {

struct CppelError : public std::runtime_error {
  const std::string type;
  const std::string message;

  explicit CppelError(const std::string& type, const std::string& message)
      : std::runtime_error("[cppel.exception." + type + "] " + message),
        type(type),
        message(message) {}
};

struct TokenError : public CppelError {
  explicit TokenError(const std::string& message)
      : CppelError("token_error", message) {}
};

#define CPPEL_THROW(exception) throw exception

}  // namespace cppel