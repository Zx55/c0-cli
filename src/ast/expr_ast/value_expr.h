/*
 * Created by zx5 on 2019/11/28.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_VALUE_EXPR_H
#define C0_VALUE_EXPR_H

/* Contains left-value and right-value */

#include "basic_expr.h"

namespace cc0::ast {
    class Int32ExprAST final: public ExprAST {
    public:
        explicit Int32ExprAST(int32_t value): ExprAST(Type::INT, value) { }
    };

    class Float64ExprAST final: public ExprAST {
    public:
        explicit Float64ExprAST(double value): ExprAST(Type::DOUBLE, value) { }
    };

    class CharExprAST final: public ExprAST {
    public:
        explicit CharExprAST(char value): ExprAST(Type::INT, static_cast<int32_t>(value)) { }
    };

    class StringExprAST final: public ExprAST {
    public:
        explicit StringExprAST(std::string value): ExprAST(Type::STRING, value) { }
    };

    class IdExprAST final: public ExprAST {
    private:
        Token _id;

    public:
        explicit IdExprAST(Token id): ExprAST(), _id(std::move(id)) { }
    };
}

#endif //C0_VALUE_EXPR_H
