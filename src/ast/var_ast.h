/*
 * Created by zx5 on 2019/11/27.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_VAR_AST_H
#define C0_VAR_AST_H

#include "basic_ast.h"
#include "expr_ast/expr.h"
#include "stmt_ast/stmt.h"

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

        void graphize(std::ostream& out, int t) override {
            out << "<var-decl> [type] " << (_const ? "const " : "") << type_str(_type) << "\n";
            out << (_init == nullptr ? _end(t) : _mid(t));
            _id->graphize(out, t + 1);
            if (_init != nullptr) {
                out << _end(t);
                _init->graphize(out, t + 1);
            }
        }
    };
}

#endif //C0_VAR_AST_H
