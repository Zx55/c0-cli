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
        explicit RootAST(range_t range, _ptrs<VarDeclAST> vars, _ptrs<FuncDefAST> funcs):
            AST(range), _vars(std::move(vars)), _funcs(std::move(funcs)) { }

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

            _graphize_list(_funcs, out, t, t + 1);
        }

        [[nodiscard]] _GenResult generate(_GenParam param) override {
            auto slot = param._slot;
            uint32_t len = 0;

            // global vars
            for (const auto& var: _vars) {
                auto res = var->generate({ param._level, 0, slot, Type::UNDEFINED });
                if (res._len != 0) slot += _make_slot(var->get_type());
                len += res._len;
            }

            // function define
            for (const auto& func: _funcs) {
                auto res = func->generate({ param._level + 1, 0, 0, func->get_ret() });
                len += res._len;
            }

            return { len, {}, {} };
        }
    };
}

#endif //C0_ROOT_AST_H
