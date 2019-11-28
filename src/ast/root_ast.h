/*
 * Created by zx5 on 2019/11/28.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_ROOT_AST_H
#define C0_ROOT_AST_H

#include "basic_ast.h"
#include "var_ast.h"
#include "func_ast.h"

#include "tools/alias.h"

#include <vector>
#include <memory>

namespace cc0::ast {
    class RootAST: public AST {
    private:
        _ptrs<DeclAST> _vars;
        _ptrs<FuncDefAST> _funcs;

    public:
        explicit RootAST(_ptrs<DeclAST> vars, _ptrs<FuncDefAST> funcs):
            _vars(std::move(vars)), _funcs(std::move(funcs)) { }
    };
}

#endif //C0_ROOT_AST_H
