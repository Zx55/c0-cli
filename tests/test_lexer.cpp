/*
 * Created by zx5 on 2019/11/22.
 * Copyright (c) zx5. All rights reserved.
 */

#include "lexer/lexer.h"
#include "ctx/src_ctx.h"
#include "ctx/rt_ctx.h"

#include "catch2/catch.hpp"

#include <sstream>
#include <vector>

#define _token(_type, _value, _sr, _sc, _er, _ec) \
    cc0::Token(_type, _value, { { _sr, _sc }, { _er, _ec }})

TEST_CASE("Test 0: Base") {
    std::string in =
            "int main() {\n"
            "    int i = 0, sum = 0;\n"
            "    while (i < 10) {\n"
            "        sum = sum + i;\n"
            "        i = i + 1;\n"
            "    }\n"
            "    print(sum);\n"
            "    return 0;\n"
            "}";
    std::istringstream ss;
    ss.str(in);

    cc0::SourceContext::set_ctx(ss);
    auto lexer = cc0::Lexer();
    lexer.all_tokens();

    if (auto errs = cc0::RuntimeContext::get_fatal(); !errs.empty())
        FAIL();
    auto output = cc0::RuntimeContext::get_tokens();
    auto ans = std::vector<cc0::Token> {
            _token(cc0::TokenType::INT, std::string("int"), 0, 0, 0, 3),
            _token(cc0::TokenType::IDENTIFIER, std::string("main"), 0, 4, 0, 8),
            _token(cc0::TokenType::LPARENTHESIS, std::string("("), 0, 8, 0, 9),
            _token(cc0::TokenType::RPARENTHESIS, std::string(")"), 0, 9, 0, 10),
            _token(cc0::TokenType::LBRACE, std::string("{"), 0, 11, 0, 12),
            _token(cc0::TokenType::INT, std::string("int"), 1, 4, 1, 7),
            _token(cc0::TokenType::IDENTIFIER, std::string("i"), 1, 8, 1, 9),
            _token(cc0::TokenType::ASSIGN, std::string("="), 1, 10, 1, 11),
            _token(cc0::TokenType::DECIMAL, 0, 1, 12, 1, 13),
            _token(cc0::TokenType::COMMA, std::string(","), 1, 13, 1, 14),
            _token(cc0::TokenType::IDENTIFIER, std::string("sum"), 1, 15, 1, 18),
            _token(cc0::TokenType::ASSIGN, std::string("="), 1, 19, 1, 20),
            _token(cc0::TokenType::DECIMAL, 0, 1, 21, 1, 22),
            _token(cc0::TokenType::SEMI, std::string(";"), 1, 22, 1, 23),
            _token(cc0::TokenType::WHILE, std::string("while"), 2, 4, 2, 9),
            _token(cc0::TokenType::LPARENTHESIS, std::string("("), 2, 10, 2, 11),
            _token(cc0::TokenType::IDENTIFIER, std::string("i"), 2, 11, 2, 12),
            _token(cc0::TokenType::LESS, std::string("<"), 2, 13, 2, 14),
            _token(cc0::TokenType::DECIMAL, 10, 2, 15, 2, 17),
            _token(cc0::TokenType::RPARENTHESIS, std::string(")"), 2, 17, 2, 18),
            _token(cc0::TokenType::LBRACE, std::string("{"), 2, 19, 2, 20),
            _token(cc0::TokenType::IDENTIFIER, std::string("sum"), 3, 8, 3, 11),
            _token(cc0::TokenType::ASSIGN, std::string("="), 3, 12, 3, 13),
            _token(cc0::TokenType::IDENTIFIER, std::string("sum"), 3, 14, 3, 17),
            _token(cc0::TokenType::PLUS, std::string("+"), 3, 18, 3, 19),
            _token(cc0::TokenType::IDENTIFIER, std::string("i"), 3, 20, 3, 21),
            _token(cc0::TokenType::SEMI, std::string(";"), 3, 21, 3, 22),
            _token(cc0::TokenType::IDENTIFIER, std::string("i"), 4, 8, 4, 9),
            _token(cc0::TokenType::ASSIGN, std::string("="), 4, 10, 4, 11),
            _token(cc0::TokenType::IDENTIFIER, std::string("i"), 4, 12, 4, 13),
            _token(cc0::TokenType::PLUS, std::string("+"), 4, 14, 4, 15),
            _token(cc0::TokenType::DECIMAL, 1, 4, 16, 4, 17),
            _token(cc0::TokenType::SEMI, std::string(";"), 4, 17, 4, 18),
            _token(cc0::TokenType::RBRACE, std::string("}"), 5, 4, 5, 5),
            _token(cc0::TokenType::PRINT, std::string("print"), 6, 4, 6, 9),
            _token(cc0::TokenType::LPARENTHESIS, std::string("("), 6, 9, 6, 10),
            _token(cc0::TokenType::IDENTIFIER, std::string("sum"), 6, 10, 6, 13),
            _token(cc0::TokenType::RPARENTHESIS, std::string(")"), 6, 13, 6, 14),
            _token(cc0::TokenType::SEMI, std::string(";"), 6, 14, 6, 15),
            _token(cc0::TokenType::RETURN, std::string("return"), 7, 4, 7, 10),
            _token(cc0::TokenType::DECIMAL, 0, 7, 11, 7, 12),
            _token(cc0::TokenType::SEMI, std::string(";"), 7, 12, 7, 13),
            _token(cc0::TokenType::RBRACE, std::string("}"), 8, 0, 8, 1)
    };

    REQUIRE(ans == output);
}
