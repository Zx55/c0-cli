/*
 * Created by zx5 on 2019/11/25.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_TOKEN_H
#define C0_TOKEN_H

#include "tools/alias.h"

#include "fmt/format.h"

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <any>

namespace cc0 {
    // 类别表
    enum class TokenType: char {
        UNDEFINED,

        CONST,
        VOID,
        INT,
        CHAR,
        DOUBLE,
        STRUCT,
        IF,
        ELSE,
        SWITCH,
        CASE,
        DEFAULT,
        WHILE,
        FOR,
        DO,
        RETURN,
        BREAK,
        CONTINUE,
        PRINT,
        SCAN,

        DECIMAL,
        HEXADECIMAL,
        FLOAT,
        IDENTIFIER,
        PLUS,
        MINUS,
        MULTIPLY,
        DIVISION,
        COMMENT,
        ASSIGN,

        EQUAL,
        GREATER,
        GREATER_EQUAL,
        LESS,
        LESS_EQUAL,
        NEQUAL,

        SEMI,
        COLON,
        COMMA,

        LPARENTHESIS,
        RPARENTHESIS,
        LBRACE,
        RBRACE,

        CHAR_LITERAL,
        STRING_LITERAL,
    };

    class Token final {
        friend std::ostream& operator<<(std::ostream& out, const Token& token) {
            out << fmt::format("{}", token);
            return out;
        }

        friend std::ostream& operator<<(std::ostream& out, const std::vector<Token>& tks) {
            if (tks.empty()) {
                out << "Bad bad - no token :(" << std::endl;
                return out;
            }

            out << fmt::format("|{:<15s}|{:<10s}|{:<11s}|{:<11s}|\n", "token", "value", "start", "end");
            out << std::string(52, '-') << std::endl;

            for (const auto& tk: tks)
                out << tk << std::endl;
            return out;
        }

        friend inline void swap(Token& lhs, Token& rhs) {
            std::swap(lhs._type, rhs._type);
            std::swap(lhs._value, rhs._value);
            std::swap(lhs._range, rhs._range);
        }

    private:
        TokenType _type;
        std::any _value;
        range_t _range;

    public:
        Token(): _type(TokenType::UNDEFINED), _range({ {0, 0}, {0, 0} }) { }
        Token(TokenType type, std::any value, range_t range):
            _type(type), _value(std::move(value)), _range(std::move(range)) { }
        Token(TokenType type, std::any value, pos_t start, pos_t end):
            _type(type), _value(std::move(value)), _range({ start, end }) { }
        Token(TokenType type, std::any value, uint64_t start_row, uint64_t start_col,
            uint64_t end_row, uint64_t end_col):
            _type(type), _value(std::move(value)), _range({ { start_row, start_col }, { end_row, end_col } }) { }

        Token(const Token& t) = default;
        Token(Token&& t) noexcept:
            _type(t._type), _value(std::move(t._value)), _range(std::move(t._range)) { }
        Token& operator=(Token t) {
            swap(*this, t);
            return *this;
        }
        bool operator==(const Token& rhs) const {
            return _type == rhs._type && get_value_string() == rhs.get_value_string()
                && _range == rhs._range;
        }

        [[nodiscard]] inline TokenType get_type() const { return _type; }
        [[nodiscard]] inline std::any get_value() const { return _value; }
        [[nodiscard]] inline range_t get_range() const { return _range; }
        [[nodiscard]] inline pos_t get_start() const { return _range.first; }
        [[nodiscard]] inline pos_t get_end() const { return _range.second; }

        [[nodiscard]] std::string get_value_string() const {
            try {
                return std::any_cast<std::string>(_value);
            } catch (const std::bad_any_cast &) { }
            try {
                return std::to_string(std::any_cast<int32_t>(_value));
            } catch (const std::bad_any_cast &) { }
            try {
                return std::to_string(std::any_cast<double>(_value));
            } catch (const std::bad_any_cast &) { }
            try {
                return std::string(1, std::any_cast<char>(_value));
            } catch (const std::bad_any_cast &) { }
            return "Invalid";
        }

        [[nodiscard]] static inline std::optional<TokenType> get_reserved(const std::string &token) {
            static std::unordered_map<std::string, TokenType> reserved{
                    { "const",      TokenType::CONST },
                    { "void",       TokenType::VOID },
                    { "int",        TokenType::INT },
                    { "char",       TokenType::CHAR },
                    { "double",     TokenType::DOUBLE },
                    { "struct",     TokenType::STRUCT },
                    { "if",         TokenType::IF },
                    { "else",       TokenType::ELSE },
                    { "switch",     TokenType::SWITCH },
                    { "case",       TokenType::CASE },
                    { "default",    TokenType::DEFAULT },
                    { "while",      TokenType::WHILE },
                    { "for",        TokenType::FOR },
                    { "do",         TokenType::DO },
                    { "return",     TokenType::RETURN },
                    { "break",      TokenType::BREAK },
                    { "continue",   TokenType::CONTINUE },
                    { "print",      TokenType::PRINT },
                    { "scan",       TokenType::SCAN },
            };

            if (const auto it = reserved.find(token); it != reserved.cend())
                return it->second;
            return std::nullopt;
        }
    };
}

namespace fmt {
    template<>
    struct formatter<cc0::Token> {
        constexpr auto parse(format_parse_context& ctx) {
            return ctx.begin();
        }

        template<typename FormatContext>
        auto format(const cc0::Token& token, FormatContext& ctx) {
            static std::vector<std::string> tks_printable = {
                    "UNDEFINED",

                    "CONST",
                    "VOID",
                    "INT",
                    "CHAR",
                    "DOUBLE",
                    "STRUCT",
                    "IF",
                    "ELSE",
                    "SWITCH",
                    "CASE",
                    "DEFAULT",
                    "WHILE",
                    "FOR",
                    "DO",
                    "RETURN",
                    "BREAK",
                    "CONTINUE",
                    "PRINT",
                    "SCAN",

                    "DECIMAL",
                    "HEXADECIMAL",
                    "FLOAT",
                    "IDENTIFIER",
                    "PLUS",
                    "MINUS",
                    "MULTIPLY",
                    "DIVISION",
                    "COMMENT",
                    "ASSIGN",

                    "EQUAL",
                    "GREATER",
                    "GREATER_EQUAL",
                    "LESS",
                    "LESS_EQUAL",
                    "NEQUAL",

                    "SEMI",
                    "COLON",
                    "COMMA",

                    "LPARENTHESIS",
                    "RPARENTHESIS",
                    "LBRACE",
                    "RBRACE",

                    "CHAR_LITERAL",
                    "STRING_LITERAL",
            };

            auto value_str = token.get_value_string();
            if (value_str.size() > 10) {
                value_str = value_str.substr(0, 7) + "...";
            }

            return format_to(ctx.out(), "|{:<15s}|{:<10s}| ({:>3d},{:>3d}) | ({:>3d},{:3d}) |",
                    tks_printable[static_cast<int>(token.get_type())], value_str,
                    token.get_start().first, token.get_start().second, token.get_end().first,
                    token.get_end().second);
        }
    };
}

#endif //C0_TOKEN_H
