/*
 * Created by zx5 on 2019/11/22.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_LEXER_H
#define C0_LEXER_H

#include "ctx/rt_ctx.h"

#include <sstream>
#include <optional>

namespace cc0 {
    class Lexer final {
    private:
        enum class DFAState: unsigned char {
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
        std::vector<std::string>& _in;
        // 存储读到的字符来拼接成_token.value
        std::stringstream _ss;
        char _ch;
        // 输入流的行列记录
        pos_t _ptr;

    public:
        Lexer(): _in(SourceContext::get_source()), _ch('0'), _ptr({ 0, -1 }) { }
        Lexer(const Lexer&) = delete;
        Lexer(Lexer&&) = delete;
        Lexer& operator=(Lexer) = delete;

        void next_token();
        void all_tokens();

    private:
        struct _ParseResult final {
            std::optional<Token> _token;
            std::optional<C0Err> _err;
            std::optional<C0Err> _wrn;

            _ParseResult(std::optional<Token> token, std::optional<C0Err> err, std::optional<C0Err> wrn):
                _token(std::move(token)), _err(std::move(err)), _wrn(std::move(wrn)) { }
        };

        inline void _get();
        inline void _unget();
        [[nodiscard]] inline _ParseResult _parse_int(pos_t p, int base) const;
        [[nodiscard]] inline _ParseResult _parse_float(pos_t p) const;

        [[nodiscard]] _ParseResult _next();
    };
}

#endif //C0_LEXER_H
