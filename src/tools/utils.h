/*
 * Created by zx5 on 2019/11/28.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_UTILS_H
#define C0_UTILS_H

#include "enums.h"

#include "lexer/token.h"

namespace cc0::ast {
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
}

#endif //C0_UTILS_H
