cmake_minimum_required(VERSION 3.17)

project(cppel)
include(FetchContent)

set(CMAKE_CXX_STANDARD 17)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)




add_executable(main main.cpp include/tokenizer.hpp)

target_link_libraries(main PRIVATE nlohmann_json::nlohmann_json)