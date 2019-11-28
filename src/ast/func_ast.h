/*
 * Created by zx5 on 2019/11/28.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_FUNC_AST_H
#define C0_FUNC_AST_H

#include "basic_ast.h"
#include "var_ast.h"
#include "expr_ast/expr.h"
#include "stmt_ast/stmt.h"

#include "tools/alias.h"
#include "tools/enums.h"
#include "tools/utils.h"
#include "lexer/token.h"

namespace cc0::ast {
    class ParamAST final: public AST {
    private:
        Type _type;
        _ptr<IdExprAST> _id;
        bool _const;

    public:
        explicit ParamAST(Type type, _ptr<IdExprAST> id, bool f_const = false):
            _type(type), _id(std::move(id)), _const(f_const) { }
    };

    class BlockStmtAST final: public StmtAST {
    private:
        _ptrs<VarDeclAST> _vars;
        _ptrs<StmtAST> _stmts;

    public:
        explicit BlockStmtAST(_ptrs<VarDeclAST> vars, _ptrs<StmtAST> stmts):
            _vars(std::move(vars)), _stmts(std::move(stmts)) { }
        BlockStmtAST(BlockStmtAST&& ast) noexcept:
            _vars(std::move(ast._vars)), _stmts(std::move(ast._stmts)) { }
    };

    class FuncDefAST final: public AST {
    private:
        Type _ret;
        _ptr<IdExprAST> _id;
        _ptrs<ParamAST> _params;
        _ptr<BlockStmtAST> _block;

    public:
        explicit FuncDefAST(Type ret, _ptr<IdExprAST> id, _ptrs<ParamAST> params, _ptr<BlockStmtAST> block):
            _ret(ret), _id(std::move(id)), _params(std::move(params)), _block(std::move(block)) { }
    };

    class FuncCallAST final: public ExprAST, public StmtAST {
    private:
        _ptr<IdExprAST> _id;
        _ptrs<ExprAST> _params;

    public:
        explicit FuncCallAST(_ptr<IdExprAST> id, _ptrs<ExprAST> params):
            ExprAST(), _id(std::move(id)), _params(std::move(params)) { }
    };
}

#endif //C0_FUNC_AST_H
