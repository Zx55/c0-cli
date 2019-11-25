/*
 * Created by zx5 on 2019/11/25.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_TOKEN_H
#define C0_TOKEN_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>
#include <any>

namespace cc0 {
    using int32_t = std::int32_t;
    using uint32_t = std::uint32_t;
    using int64_t = std::int64_t;
    using uint64_t = std::uint64_t;

    using pos_t = std::pair<uint64_t, uint64_t>;



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



    // 保留字表
    const auto reserved = std::unordered_map<std::string, TokenType>{
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

    class Token final {
        friend std::ostream& operator<<(std::ostream& out, const Token& token) {
            auto tks_printable = std::vector<std::string>({
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
            });

            out << "Type: " << tks_printable[static_cast<int>(token._type)] << "  Value: " << token.get_value_string()
                << "  Start: (" << token._start.first << ", " << token._start.second << ")  End: ("
                << token._end.first << ", " << token._end.second << ")";
            return out;
        }

        friend inline void swap(Token& lhs, Token& rhs) {
            std::swap(lhs._type, rhs._type);
            std::swap(lhs._value, rhs._value);
            std::swap(lhs._start, rhs._start);
            std::swap(lhs._end, rhs._end);
        }

    private:
        TokenType _type;
        std::any _value;
        pos_t _start;
        pos_t _end;

    public:
        Token(): _type(TokenType::UNDEFINED), _start({ 0, 0 }), _end({ 0, 0 }) { }
        Token(TokenType type, std::any value, pos_t start, pos_t end):
            _type(type), _value(std::move(value)), _start(std::move(start)), _end(std::move(end)) { }
        Token(TokenType type, std::any value, uint64_t start_row, uint64_t start_col,
            uint64_t end_row, uint64_t end_col):
            _type(type), _value(std::move(value)), _start({ start_row, start_col }), _end({ end_row, end_col }) { }

        Token(const Token& t) = default;
        Token(Token&& t) noexcept: _type(t._type), _value(std::move(t._value)), _start(std::move(t._start)),
            _end(std::move(t._end)) { }
        Token& operator=(Token t) {
            swap(*this, t);
            return *this;
        }
        bool operator==(Token rhs) {
            return _type == rhs._type && get_value_string() == rhs.get_value_string()
                && _start == rhs._start && _end == rhs._end;
        }

        [[nodiscard]] TokenType get_type() const { return _type; }
        [[nodiscard]] std::any get_value() const { return _value; }
        [[nodiscard]] pos_t get_start() const { return _start; }
        [[nodiscard]] pos_t get_end() const { return _end; }

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
    };
}

#endif //C0_TOKEN_H
