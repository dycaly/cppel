### What's this
a c++ simple implement of spring expression

### How to use
```c++
using json = nlohmann::json;
json data = "{\"names\": \"Jack,Rose\"}"_json;

cppel::EvaluationContext evaluation_context(data);
std::string expr_str = "#split(names, ',')";

cppel::Parser parser;
cppel::Expression expr = parser.parse(expr_str);

std::shared_ptr<json> rlt = expr.evaluate(evaluation_context);

std::cout << rlt.get()->dump() << std::endl; // ["Jack","Rose"]
```