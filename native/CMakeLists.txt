cmake_minimum_required(VERSION 3.0)
project (mini)
file(GLOB_RECURSE core "core/*.c")
file(GLOB_RECURSE collections "collections/*.c")
file(GLOB_RECURSE generated "generated/*.c")
file(GLOB_RECURSE ast "ast/*.c")
file(GLOB_RECURSE minic "minic/*.c")
file(GLOB_RECURSE mini "mini/*.c")
file(GLOB_RECURSE micro "micro/*.c")
file(GLOB_RECURSE test "test/*.c")
include_directories(.)
add_executable(mini ${core} ${collections} ${generated} ${ast} ${minic} ${mini})
add_executable(micro ${core} ${collections} ${generated} ${ast} ${minic} ${micro} ast/read_ast.c ast/read_ast.h ast/expression.c ast/expression.h ast/symbol_for_name.c ast/symbol_for_name.h ast/symbol.h ast/symbol.c)
add_executable(tests ${test} ${core} ${collections} ${minic} ${generated})
set(CMAKE_C_FLAGS "-std=c11 -O0 -g3 -Wall -Wpedantic")
