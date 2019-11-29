/*
 * Created by zx5 on 2019/11/28.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_ARITH_EXPR_H
#define C0_ARITH_EXPR_H

#include "basic_expr.h"

namespace cc0::ast {
    class BinaryExprAST final: public ExprAST {
    private:
        _ptr<ExprAST> _lhs;
        Op _op;
        _ptr<ExprAST> _rhs;

    public:
        explicit BinaryExprAST(_ptr<ExprAST> lhs, Op op, _ptr<ExprAST> rhs):
            ExprAST(), _lhs(std::move(lhs)), _op(op), _rhs(std::move(rhs)) { }
    };

    class UnaryExprAST final: public ExprAST {
    private:
        bool _sign; // true => '+'; false => '-'
        _ptr<ExprAST> _expr;

    public:
        explicit UnaryExprAST(bool sign, _ptr<ExprAST> expr):
            _sign(sign), _expr(std::move(expr)) { }
    };
}

#endif //C0_ARITH_EXPR_H
