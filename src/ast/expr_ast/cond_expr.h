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
        explicit CondExprAST(range_t range, _ptr<ExprAST> lhs):
                ExprAST(range, Type::INT), _lhs(std::move(lhs)), _op(Op::NEQ) {
            _rhs = std::make_unique<Int32ExprAST>(range, 0);
        }
        explicit CondExprAST(range_t range, _ptr<ExprAST> lhs, Op op, _ptr<ExprAST> rhs):
            ExprAST(range, Type::INT), _lhs(std::move(lhs)), _op(op), _rhs(std::move(rhs)) { }

        [[nodiscard]] inline Op get_op() const { return _op; }

        void graphize(std::ostream& out, int t) override {
            out << "<cond-expr> [op] " << _op_str(_op) << "\n" << _mid(t);
            _lhs->graphize(out, t + 1);
            out << _end(t);
            _rhs->graphize(out, t + 1);
        }

        _GenResult generate(_GenParam param) override {

        }
    };
}

#endif //C0_COND_EXPR_H
