/*
 * Created by zx5 on 2019/11/28.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_LOOP_STMT_H
#define C0_LOOP_STMT_H

#include "basic_stmt.h"

namespace cc0::ast {
    class WhileStmtAST: public StmtAST {
    private:
        _ptr<CondExprAST> _cond;
        _ptrs<StmtAST> _stmts;

    public:
        explicit WhileStmtAST(_ptr<CondExprAST> cond, _ptrs<StmtAST> stmts):
            _cond(std::move(cond)), _stmts(std::move(stmts)) { }
    };

    /*
     * class ForStmtAST: public StmtAST { };
     *
     * class DoWhileStmtAST: public StmtAST { };
     */
}

#endif //C0_LOOP_STMT_H
