#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <cppel/expression.hpp>
#include <cppel/parser.hpp>

int main() {
  using json = nlohmann::json;

  std::string expr_str = "#split(names, ',')";
  cppel::Parser parser;
  cppel::Expression expr = parser.parse(expr_str);

  json data = "{\"names\": \"Jack,Rose\"}"_json;
  cppel::EvaluationContext evaluation_context(data);

  std::shared_ptr<json> rlt = expr.evaluate(evaluation_context);
  std::cout << rlt->dump() << std::endl;

  return 0;
}