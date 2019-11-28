/*
 * Created by zx5 on 2019/11/28.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_FUNC_AST_H
#define C0_FUNC_AST_H

#include "basic_ast.h"
#include "expr_ast/expr.h"
#include "stmt_ast/stmt.h"

#include "tools/alias.h"
#include "tools/enums.h"
#include "tools/utils.h"
#include "lexer/token.h"

namespace cc0::ast {
    class ParamAST: public AST {
    private:
        Type _type;
        _ptr<IdExprAST> _id;
        bool _const;

    public:
        explicit ParamAST(TokenType type, _ptr<IdExprAST> id, bool f_const = false):
            _type(make_type(type)), _id(std::move(id)), _const(f_const) { }
    };

    class FuncDefAST: public AST {
    private:
        Type _ret;
        _ptr<IdExprAST> _id;
        _ptrs<ParamAST> _params;
        _ptrs<StmtAST> _stmts;

    public:
        explicit FuncDefAST(TokenType ret, _ptr<IdExprAST> id, _ptrs<ParamAST> params, _ptrs<StmtAST> stmts):
            _ret(make_type(ret)), _id(std::move(id)), _params(std::move(params)), _stmts(std::move(stmts)) { }
    };

    class FuncCallAST: public ExprAST, public StmtAST {
    private:
        _ptr<IdExprAST> _id;
        _ptrs<ExprAST> _params;

    public:
        explicit FuncCallAST(_ptr<IdExprAST> id, _ptrs<ExprAST> params):
            ExprAST(), _id(std::move(id)), _params(std::move(params)) { }
    };
}

#endif //C0_FUNC_AST_H
