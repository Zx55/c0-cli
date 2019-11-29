/*
 * Created by zx5 on 2019/11/28.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_UTILS_H
#define C0_UTILS_H

#include "enums.h"

#include "lexer/token.h"

namespace cc0 {
    inline bool istype(TokenType type) {
        switch (type) {
            case TokenType::VOID:
                [[fallthrough]];
            case TokenType::INT:
                [[fallthrough]];
            case TokenType::CHAR:
                [[fallthrough]];
            case TokenType::DOUBLE:
                return true;
            default:
                return false;
        }
    }

    inline bool isconst(TokenType type) {
        return type == TokenType::CONST;
    }

    inline bool isre_op(TokenType type) {
        switch (type) {
            case TokenType::LESS:
                [[fallthrough]];
            case TokenType::LESS_EQUAL:
                [[fallthrough]];
            case TokenType::GREATER:
                [[fallthrough]];
            case TokenType::GREATER_EQUAL:
                [[fallthrough]];
            case TokenType::EQUAL:
                [[fallthrough]];
            case TokenType::NEQUAL:
                return true;
            default:
                return false;
        }
    }

    inline bool isadd(TokenType type) {
        switch (type) {
            case TokenType::PLUS:
                [[fallthrough]];
            case TokenType::MINUS:
                return true;
            default:
                return false;
        }
    }

    inline bool ismul(TokenType type) {
        switch (type) {
            case TokenType::MULTIPLY:
                [[fallthrough]];
            case TokenType::DIVISION:
                return true;
            default:
                return false;
        }
    }

    namespace ast {
        inline Type make_type(TokenType type) {
            switch (type) {
                case TokenType::VOID:
                    return Type::VOID;
                case TokenType::INT:
                    return Type::INT;
                case TokenType::CHAR:
                    return Type::CHAR;
                case TokenType::DOUBLE:
                    return Type::DOUBLE;
                default:
                    return Type::UNDEFINED;
            }
        }

        inline Op make_op(TokenType op) {
            switch (op) {
                case TokenType::PLUS:
                    return Op::ADD;
                case TokenType::MINUS:
                    return Op::SUB;
                case TokenType::MULTIPLY:
                    return Op::MUL;
                case TokenType::DIVISION:
                    return Op::DIV;
                case TokenType::LESS:
                    return Op::LT;
                case TokenType::LESS_EQUAL:
                    return Op::LE;
                case TokenType::GREATER:
                    return Op::GT;
                case TokenType::GREATER_EQUAL:
                    return Op::GE;
                case TokenType::EQUAL:
                    return Op::EQ;
                case TokenType::NEQUAL:
                    return Op::NEQ;
                default:
                    return Op::UNDEFINED;
            }
        }

        inline int32_t make_escape(std::string ch_str) {
            switch (ch_str[1]) {
                case 'r':
                    return '\r';
                case 'n':
                    return '\n';
                case 't':
                    return '\t';
                case '\\':
                    return '\\';
                case '\'':
                    return '\'';
                case '\"':
                    return '\"';
                case 'x': {
                    auto hex_str = ch_str.substr(2);
                    try {
                        return std::stoi(hex_str, nullptr, 16);
                    } catch (const std::out_of_range&) {
                        return -1;
                    }
                }
                default:
                    return -1;
            }
        }
    }
}

#endif //C0_UTILS_H
