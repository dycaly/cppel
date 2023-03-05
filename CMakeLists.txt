cmake_minimum_required(VERSION 3.17)

project(cppel)
include(FetchContent)

set(CMAKE_CXX_STANDARD 11)

# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24:
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

include_directories(include)

add_executable(main main.cpp)

target_link_libraries(main PRIVATE nlohmann_json::nlohmann_json)