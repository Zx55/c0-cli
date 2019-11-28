/*
 * Created by zx5 on 2019/11/28.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_LOOP_STMT_H
#define C0_LOOP_STMT_H

#include "basic_stmt.h"

namespace cc0::ast {
    class WhileStmtAST final: public StmtAST {
    private:
        _ptr<CondExprAST> _cond;
        _ptr<StmtAST> _stmt;

    public:
        explicit WhileStmtAST(_ptr<CondExprAST> cond, _ptr<StmtAST> stmt):
            _cond(std::move(cond)), _stmt(std::move(stmt)) { }
    };

    /*
     * class ForStmtAST final: public StmtAST { };
     *
     * class DoWhileStmtAST final: public StmtAST { };
     */
}

#endif //C0_LOOP_STMT_H
