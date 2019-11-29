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
        explicit CastExprAST(Type cast, _ptr<ExprAST> expr):
            _cast(cast), _expr(std::move(expr)) { }
    };
}

#endif //C0_CAST_EXPR_H
