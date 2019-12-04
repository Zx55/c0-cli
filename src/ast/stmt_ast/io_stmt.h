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
        explicit PrintStmtAST(range_t range, _ptrs<ExprAST> printable):
            StmtAST(range), _printable(std::move(printable)) { }

        void graphize(std::ostream& out, int t) override {
            out << "<print-stmt>\n";
            _graphize_list(_printable, out, t, t + 1);
        }

        _GenResult generate(_GenParam param) override {

        }
    };

    class ScanStmtAST final: public StmtAST {
    private:
        _ptr<IdExprAST> _id;

    public:
        explicit ScanStmtAST(range_t range, _ptr<IdExprAST> id):
            StmtAST(range), _id(std::move(id)) { }

        void graphize(std::ostream& out, int t) override {
            out << "<scan-stmt> ";
            _id->graphize(out, t + 1);
        }

        _GenResult generate(_GenParam param) override {

        }
    };
}

#endif //C0_IO_STMT_H
