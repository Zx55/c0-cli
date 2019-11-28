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
    class VarDeclAST final: public AST {
    private:
        ast::Type _type;
        _ptr<IdExprAST> _id;
        _ptr<ExprAST> _init;
        bool _const;

    public:
        explicit VarDeclAST(Type type, _ptr<IdExprAST> id, _ptr<ExprAST> init, bool f_const = false):
            _type(type), _id(std::move(id)), _init(std::move(init)), _const(f_const) { }
    };

    class AssignAST final: public ExprAST, public StmtAST {
    private:
        _ptr<IdExprAST> _id;
        _ptr<ExprAST> _value;

    public:
        explicit AssignAST(_ptr<IdExprAST> id, _ptr<ExprAST> value):
            _id(std::move(id)), _value(std::move(value)) { }
    };
}


#endif //C0_VAR_AST_H
