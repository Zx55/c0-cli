/*
 * Created by zx5 on 2019/11/28.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_COND_STMT_H
#define C0_COND_STMT_H

#include "basic_stmt.h"

namespace cc0::ast {
    class IfElseStmtAST: public StmtAST {
    private:
        _ptr<CondExprAST> _cond;
        _ptrs<StmtAST> _true;
        _ptrs<StmtAST> _false;

    public:
        explicit IfElseStmtAST(_ptr<CondExprAST> cond, _ptrs<StmtAST> s_true, _ptrs<StmtAST> s_false):
            _cond(std::move(cond)), _true(std::move(s_true)), _false(std::move(s_false)) { }
    };

    /*
     * class SwitchStmtAST: public StmtAST { };
     */
}

#endif //C0_COND_STMT_H
