/*
 * Created by zx5 on 2019/11/22.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_LEXER_H
#define C0_LEXER_H

#include "token.h"
#include "error/error.h"

#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

namespace cc0 {
    class Lexer final {
    private:
        enum class DFAState: char {
            INIT,
            ZERO,
            DECIMAL,
            HEXADECIMAL,
            IDENTIFIER,
            FLOAT_DOT,
            FLOAT_EXP,
            PLUS,
            MINUS,
            MULTIPLY,
            DIVISION,
            SINGLE_COMMENT,
            MULTI_COMMENT,
            ASSIGN,
            GREATER,
            LESS,
            NEQUAL,
            SEMI,
            COMMA,
            COLON,
            CHAR,
            STRING,
            LPARENTHESIS,
            RPARENTHESIS,
            LBRACE,
            RBRACE,
        };

        // 输入缓冲
        std::vector<std::string> _in;
        // 存储读到的字符来拼接成_token.value
        std::stringstream _ss;
        char _ch;
        // 输入流的状态记录
        int64_t _row;
        int64_t _col;

    public:
        Lexer(std::istream& in): _ch('0'), _row(0), _col(-1) {
            std::string line;
            while (std::getline(in, line))
                _in.push_back(line + '\n');
        }
        Lexer(const Lexer &) = delete;
        Lexer(Lexer &&) = delete;
        Lexer& operator=(Lexer) = delete;

        [[nodiscard]] std::vector<std::string> get_input() { return _in; }
        [[nodiscard]] std::string get_line(int64_t row) { return _in.at(row); }
        [[nodiscard]] std::pair<std::optional<Token>, std::optional<C0Err>> next_token();
        [[nodiscard]] std::pair<std::vector<Token>, std::vector<C0Err>> all_tokens();

    private:
        inline void _get();
        inline void _unget();
        [[nodiscard]] inline std::pair<std::optional<Token>, std::optional<C0Err>>
            _parse_int(pos_t p, int base) const;
        [[nodiscard]] inline std::pair<std::optional<Token>, std::optional<C0Err>>
            _parse_float(pos_t p) const;
    };
}

#endif //C0_LEXER_H
