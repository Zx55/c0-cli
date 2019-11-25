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
            FLOAT_NO_INT, // .123 => FLOAT_NO_INT
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

        // FIXME: 重写缓冲 1) row和col有bug  2) 留备份，报错能有详细信息
        // 输入流 默认是std::cin
        std::istream &_in;
        // 存储读到的字符来拼接成_token.value
        std::stringstream _ss;
        char _ch;
        // 输入流的状态记录
        uint64_t _row;
        uint64_t _col;
        std::vector<uint64_t> _line_size;

    public:
        Lexer(std::istream &in = std::cin): _in(in), _ch('\0'), _row(0), _col(0) { }
        Lexer(const Lexer &) = delete;
        Lexer(Lexer &&) = delete;
        Lexer& operator=(Lexer) = delete;

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
