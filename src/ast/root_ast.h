/*
 * Created by zx5 on 2019/11/28.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_ROOT_AST_H
#define C0_ROOT_AST_H

#include "basic_ast.h"
#include "var_ast.h"
#include "func_ast.h"

namespace cc0::ast {
    class RootAST final: public AST {
    private:
        _ptrs<VarDeclAST> _vars;
        _ptrs<FuncDefAST> _funcs;

    public:
        explicit RootAST(_ptrs<VarDeclAST> vars, _ptrs<FuncDefAST> funcs):
            _vars(std::move(vars)), _funcs(std::move(funcs)) { }

        void graphize(std::ostream& out, int t) override {
            out << "<c0-root>\n";

            if (!_vars.empty()) {
                for (auto it = _vars.cbegin(); it != _vars.cend() - 1; ++it) {
                    out << _mid(t);
                    (*it)->graphize(out, t + 1);
                }
                if (!_funcs.empty()) out << _mid(t);
                else out << _end(t);
                (*(_vars.cend() - 1))->graphize(out, t + 1);
            }

            if (!_funcs.empty()) {
                for (auto it = _funcs.cbegin(); it != _funcs.cend() - 1; ++it) {
                    out << _mid(t);
                    (*it)->graphize(out, t + 1);
                }
                out << _end(t);
                (*(_funcs.cend() - 1))->graphize(out, t + 1);
            }
        }
    };
}

#endif //C0_ROOT_AST_H
