### What's this?
a simple c++ implementation of spring expression

### How to use?
```c++
#include <iostream>
#include <nlohmann/json.hpp>
#include "include/expression.hpp"
#include "include/parser.hpp"
  
using json = nlohmann::json;

std::string expr_str = "#split(names, ',')";
cppel::Parser parser;
cppel::Expression expr = parser.parse(expr_str);

json data = "{\"names\": \"Jack,Rose\"}"_json;
cppel::EvaluationContext evaluation_context(data);

std::shared_ptr<json> rlt = expr.evaluate(evaluation_context);
std::cout << rlt->dump() << std::endl;
```