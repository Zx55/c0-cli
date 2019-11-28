/*
 * Created by zx5 on 2019/11/28.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_COND_STMT_H
#define C0_COND_STMT_H

#include "basic_stmt.h"

namespace cc0::ast {
    class IfElseStmtAST final: public StmtAST {
    private:
        _ptr<CondExprAST> _cond;
        _ptr<StmtAST> _true;
        _ptr<StmtAST> _false;

    public:
        explicit IfElseStmtAST(_ptr<CondExprAST> cond, _ptr<StmtAST> s_true, _ptr<StmtAST> s_false):
            _cond(std::move(cond)), _true(std::move(s_true)), _false(std::move(s_false)) { }
    };

    /*
     * class SwitchStmtAST final: public StmtAST { };
     */
}

#endif //C0_COND_STMT_H
