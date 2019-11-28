/*
 * Created by zx5 on 2019/11/28.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_IO_STMT_H
#define C0_IO_STMT_H

#include "basic_stmt.h"

namespace cc0::ast {
    class PrintStmtAST final: StmtAST {
    private:
        _ptrs<ExprAST> _printable;

    public:
        explicit PrintStmtAST(_ptrs<ExprAST> printable): _printable(std::move(printable)) { }
    };

    class ScanStmtAST final: StmtAST {
    private:
        _ptr<IdExprAST> _id;

    public:
        explicit ScanStmtAST(_ptr<IdExprAST> id): _id(std::move(id)) { }
    };
}

#endif //C0_IO_STMT_H
