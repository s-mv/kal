cmake_minimum_required(VERSION 3.25)

project(kal VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(kal main.cpp)
add_library(frontend src/lexer.cpp src/parser.cpp src/codegen.cpp)

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
target_link_libraries(frontend PRIVATE LLVM-22)

# add_library(cursor src/cursor.cpp)

target_include_directories(frontend PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(kal PRIVATE frontend)
