/*
 * Created by zx5 on 2019/11/28.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_COND_EXPR_H
#define C0_COND_EXPR_H

#include "basic_expr.h"
#include "value_expr.h"

namespace cc0::ast {
    class CondExprAST final: public ExprAST {
    private:
        _ptr<ExprAST> _lhs;
        Op _op;
        _ptr<ExprAST> _rhs;

    public:
        explicit CondExprAST(_ptr<ExprAST> lhs, Op op = Op::NEQ, _ptr<ExprAST> rhs = _ptr<ExprAST>(new Int32ExprAST(0))):
            ExprAST(Type::INT), _lhs(std::move(lhs)), _op(op), _rhs(std::move(rhs)) { }
    };
}

#endif //C0_COND_EXPR_H
