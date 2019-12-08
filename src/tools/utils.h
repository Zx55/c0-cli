/*
 * Created by zx5 on 2019/11/28.
 * Copyright (c) zx5. All rights reserved.
 */

#pragma once
#ifndef C0_UTILS_H
#define C0_UTILS_H

#include "enums.h"

#include <cctype>

#define IS_FUNC(fn) \
    inline bool fn(int ch) { \
        return std::fn(static_cast<unsigned char>(ch)); \
    } \

namespace cc0::utils {
    IS_FUNC(isprint)
    IS_FUNC(isblank)
    IS_FUNC(isdigit)
    IS_FUNC(isalpha)
    IS_FUNC(isalnum)
    IS_FUNC(ispunct)

    inline bool isline(int ch) {
        switch (ch) {
            case '\r':
                [[fallthrough]];
            case '\n':
                return true;
            default:
                return false;
        }
    }

    inline bool isspace(int ch) {
        return isblank(ch) || isline(ch);
    }

    inline bool ishex(int ch) {
        return isdigit(ch) || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
    }

    inline bool iseof(int ch) {
        return ch == '\0';
    }

    // 可接受字符集
    inline bool isaccept(int ch) {
        return isspace(ch) || isalnum(ch) || ispunct(ch);
    }

    inline bool isc_char(int ch) {
        return isaccept(ch) && !isline(ch) && ch != '\'' && ch != '\\';
    }

    inline bool iss_char(int ch) {
        return isaccept(ch) && !isline(ch) && ch != '\"' && ch != '\\';
    }

    // 转义字符 \x特判
    inline bool ise_char(int ch) {
        return ch == 'r' || ch == 'n' || ch == 't' || ch == '\\'
                || ch == '\'' || ch == '\"';
    }

    inline bool istype(TokenType type) {
        return type == TokenType::VOID || type == TokenType::INT
               || type == TokenType::CHAR || type == TokenType::DOUBLE;
    }

    inline bool isconst(TokenType type) {
        return type == TokenType::CONST;
    }

    inline bool isre_op(TokenType type) {
        return type == TokenType::LESS || type == TokenType::LESS_EQUAL
                || type == TokenType::GREATER || type == TokenType::GREATER_EQUAL
                || type == TokenType::EQUAL || type == TokenType::NEQUAL;
    }

    inline bool isadd(TokenType type) {
        return type == TokenType::PLUS || type == TokenType::MINUS;
    }

    inline bool ismul(TokenType type) {
        return type == TokenType::MULTIPLY || type == TokenType::DIVISION;
    }

    inline ast::Type make_type(TokenType type) {
        using namespace ast;

        switch (type) {
            case TokenType::VOID:   return Type::VOID;
            case TokenType::INT:    return Type::INT;
            case TokenType::CHAR:   return Type::CHAR;
            case TokenType::DOUBLE: return Type::DOUBLE;
            default:                return Type::UNDEFINED;
        }
    }

    inline ast::Op make_op(TokenType op) {
        using namespace ast;

        switch (op) {
            case TokenType::PLUS:           return Op::ADD;
            case TokenType::MINUS:          return Op::SUB;
            case TokenType::MULTIPLY:       return Op::MUL;
            case TokenType::DIVISION:       return Op::DIV;
            case TokenType::LESS:           return Op::LT;
            case TokenType::LESS_EQUAL:     return Op::LE;
            case TokenType::GREATER:        return Op::GT;
            case TokenType::GREATER_EQUAL:  return Op::GE;
            case TokenType::EQUAL:          return Op::EQ;
            case TokenType::NEQUAL:         return Op::NEQ;
            default:                        return Op::UNDEFINED;
        }
    }

    inline char make_escape(char ch) {
        switch (ch) {
            case 'r':   return '\r';
            case 'n':   return '\n';
            case 't':   return '\t';
            case '\\':  return '\\';
            case '\'':  return '\'';
            case '\"':  return '\"';
            default:    return -1;
        }
    }

    template <typename T>
    T swap_endian(T u) {
        union {
            T u;
            unsigned char u8[sizeof(T)];
        } src{}, dest{};

        src.u = u;
        for (std::size_t i = 0; i < sizeof(T); ++i)
            dest.u8[i] = src.u8[sizeof(T) - i - 1];

        return dest.u;
    }
}

#endif //C0_UTILS_H
