#pragma once
#include <stdexcept>
#include <string>

namespace cppel {

struct CppelError : public std::runtime_error {
  const std::string type;
  const std::string message;

  explicit CppelError(const std::string &type, const std::string &message)
      : std::runtime_error("[cppel.exception." + type + "] " + message),
        type(type),
        message(message) {}
};

struct TokenError : public CppelError {
  explicit TokenError(const std::string &message)
      : CppelError("token_error", message) {}
};

struct ParseError : public CppelError {
  explicit ParseError(const std::string &message)
      : CppelError("parse_error", message) {}
};

struct EvaluateError : public CppelError {
  explicit EvaluateError(const std::string &message)
      : CppelError("evaluate_error", message) {}
};

#define CPPEL_THROW(exception) throw exception

}  // namespace cppel