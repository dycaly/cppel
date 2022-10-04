#include <string>
#include <iostream>
#include <nlohmann/json.hpp>
#include "./include/expression.hpp"
#include "./include/parser.hpp"

int main() {

  using json = nlohmann::json;
  json data = "{\"names\": \"Jack,Rose\"}"_json;

  cppel::EvaluationContext evaluation_context(data);
  std::string expr_str = "#split(names, ',')";

  cppel::Parser parser;
  cppel::Expression expr = parser.parse(expr_str);

  std::shared_ptr<json> rlt = expr.evaluate(evaluation_context);

  std::cout << rlt->dump() << std::endl;

  return 0;
}