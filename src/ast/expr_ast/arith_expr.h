/*
 * Created by zx5 on 2019/11/28.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_ARITH_EXPR_H
#define C0_ARITH_EXPR_H

#include "basic_expr.h"

namespace cc0::ast {
    class BinaryExprAST: public ExprAST {
    private:
        _ptr<ExprAST> _lhs;
        Op _op;
        _ptr<ExprAST> _rhs;

    public:
        explicit BinaryExprAST(_ptr<ExprAST> lhs, Op op, _ptr<ExprAST> rhs):
            ExprAST(), _lhs(std::move(lhs)), _op(op), _rhs(std::move(rhs)) { }
    };
}

#endif //C0_ARITH_EXPR_H
