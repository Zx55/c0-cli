/*
 * Created by zx5 on 2019/11/29.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_CAST_EXPR_H
#define C0_CAST_EXPR_H

#include "basic_expr.h"

namespace cc0::ast {
    class CastExprAST final: public ExprAST {
    private:
        Type _cast;
        _ptr<ExprAST> _expr;

    public:
        explicit CastExprAST(range_t range, Type cast, _ptr<ExprAST> expr):
            ExprAST(range), _cast(cast), _expr(std::move(expr)) { }

        void graphize(std::ostream& out, int t) override {
            out << "<cast-expr> [type] " << _type_str(_cast) << "\n" << _end(t);
            _expr->graphize(out, t + 1);
        }

        _GenResult generate(_GenParam param) override {

        }
    };
}

#endif //C0_CAST_EXPR_H
