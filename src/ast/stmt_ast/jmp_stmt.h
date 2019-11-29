/*
 * Created by zx5 on 2019/11/28.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_JMP_STMT_H
#define C0_JMP_STMT_H

#include "basic_stmt.h"

namespace cc0::ast {
    class ReturnStmtAST final: public StmtAST {
    private:
        _ptr<ExprAST> _ret;

    public:
        explicit ReturnStmtAST(_ptr<ExprAST> ret = std::make_unique<ExprAST>(Type::VOID)):
            _ret(std::move(ret)) { }
    };

    class BreakStmtAST final: public StmtAST {
    public:
        explicit BreakStmtAST(): StmtAST() { }
    };

    class ContinueStmtAST final: public StmtAST {
    public:
        explicit ContinueStmtAST(): StmtAST() { }
    };
}

#endif //C0_JMP_STMT_H
