cmake_minimum_required(VERSION 3.12)

# Project name and version
project(c0 VERSION 0.0.1)

# Compiler options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")

add_subdirectory(3rd_party/fmt)
add_subdirectory(3rd_party/argparse)

# Build libc0
set(LIB "lib${PROJECT_NAME}")
# Lexer
set(LEXER_SRC
        src/lexer/lexer.cpp src/lexer/lexer.h src/lexer/token.h)
# RDP
set(ANALYSER_SRC
        src/analyser/analyser.h src/analyser/rdp.cpp src/analyser/rdp.h)
# Abstract Syntax Tree
set(AST_SRC
        src/ast/ast.h src/ast/basic_ast.h
        src/ast/root_ast.h src/ast/var_ast.h src/ast/func_ast.h
        src/ast/expr_ast/expr.h src/ast/expr_ast/basic_expr.h src/ast/expr_ast/cond_expr.h
        src/ast/expr_ast/arith_expr.h src/ast/expr_ast/value_expr.h src/ast/expr_ast/cast_expr.h
        src/ast/stmt_ast/stmt.h src/ast/stmt_ast/basic_stmt.h src/ast/stmt_ast/cond_stmt.h
        src/ast/stmt_ast/loop_stmt.h src/ast/stmt_ast/io_stmt.h src/ast/stmt_ast/jmp_stmt.h)
# Instruction
set(IST_SRC src/ist/instruction.h src/ist/generator.h)
# Symbol Table
set(TBL_SRC src/symtbl/symbol.h src/symtbl/symtbl.h)
# Context
set(CTX_SRC
        src/ctx/src_ctx.cpp src/ctx/src_ctx.h
        src/ctx/rt_ctx.cpp src/ctx/rt_ctx.h
        src/ctx/gen_ctx.cpp src/ctx/gen_ctx.h)
# Tools
set(TOOL_SRC
        src/tools/alias.h src/tools/enums.h src/tools/error.h src/tools/utils.h)

set(LIB_SRC
        ${CTX_SRC}
        ${LEXER_SRC}
        ${ANALYSER_SRC}
        ${AST_SRC}
        ${IST_SRC}
        ${TBL_SRC}
        ${TOOL_SRC}
         src/core.h)
add_library(${LIB} ${LIB_SRC})
target_include_directories(${LIB} PRIVATE
        ./src
        ./3rd_party/fmt/include)
target_link_libraries(${LIB} PUBLIC stdc++fs)

# Build executable
set(EXE ${PROJECT_NAME})
set(EXE_SRC
        src/main.cpp)
add_executable(${EXE} ${EXE_SRC})
target_include_directories(${EXE} PRIVATE ./src)
target_link_libraries(${EXE} PRIVATE ${LIB} fmt::fmt argparse)

if(MSVC)
    target_compile_options(${EXE} PRIVATE /W3)
    target_compile_options(${LIB} PRIVATE /W3)
else()
    target_compile_options(${EXE} PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(${LIB} PRIVATE -Wall -Wextra -pedantic)
endif()
