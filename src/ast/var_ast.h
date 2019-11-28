/*
 * Created by zx5 on 2019/11/27.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_VAR_AST_H
#define C0_VAR_AST_H

#include "basic_ast.h"
#include "expr_ast/expr.h"
#include "stmt_ast/stmt.h"

#include "tools/alias.h"
#include "tools/enums.h"
#include "tools/utils.h"
#include "lexer/token.h"

#include <vector>
#include <memory>
#include <optional>

namespace cc0::ast {
    class DeclAST: public AST {
    private:
        ast::Type _type;
        _ptr<IdExprAST> _id;
        _ptr<ExprAST> _init;
        bool _const;

    public:
        explicit DeclAST(TokenType type, _ptr<IdExprAST> id, _ptr<ExprAST> init, bool f_const = false):
            _type(make_type(type)), _id(std::move(id)), _init(std::move(init)), _const(f_const) { }
    };

    class AssignAST: public ExprAST, public StmtAST {
    private:
        _ptr<IdExprAST> _id;
        _ptr<ExprAST> _value;

    public:
        explicit AssignAST(_ptr<IdExprAST> id, _ptr<ExprAST> value):
            _id(std::move(id)), _value(std::move(value)) { }
    };
}


#endif //C0_VAR_AST_H
