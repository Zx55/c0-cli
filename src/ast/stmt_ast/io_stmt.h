/*
 * Created by zx5 on 2019/11/28.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_IO_STMT_H
#define C0_IO_STMT_H

#include "basic_stmt.h"

namespace cc0::ast {
    class PrintStmtAST final: public StmtAST {
    private:
        _ptrs<ExprAST> _printable;

    public:
        explicit PrintStmtAST(_ptrs<ExprAST> printable): _printable(std::move(printable)) { }

        void graphize(std::ostream& out, int t) override {
            out << "<print-stmt>\n";
            graphize_list(_printable, out, t, t + 1);
        }
    };

    class ScanStmtAST final: public StmtAST {
    private:
        _ptr<IdExprAST> _id;

    public:
        explicit ScanStmtAST(_ptr<IdExprAST> id): _id(std::move(id)) { }

        void graphize(std::ostream& out, int t) override {
            out << "<scan-stmt> ";
            _id->graphize(out, t + 1);
        }
    };
}

#endif //C0_IO_STMT_H
