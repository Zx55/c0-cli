/*
 * Created by zx5 on 2019/11/22.
 * Copyright (c) zx5. All rights reserved.
 */

#include "lexer/lexer.h"

#include "catch2/catch.hpp"
#include "fmt/format.h"

#include <sstream>
#include <vector>

TEST_CASE("Test 0: Base") {
    std::string in =
            "int main() {"
            "    int i = 0, sum = 0;"
            "    while (i < 10) {"
            "        sum = sum + i;"
            "        i = i + 1;"
            "    }"
            "    print(sum);"
            "    return 0;"
            "}";
    std::stringstream ss;
    ss.str(in);

}

TEST_CASE("Test 1: Float.") {

}