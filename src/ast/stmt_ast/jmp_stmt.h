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
            StmtAST(), _ret(std::move(ret)) { }

        void graphize(std::ostream& out, int t) override {
            out << "<ret-stmt>\n";
            if (_ret != nullptr) {
                out << _end(t);
                _ret->graphize(out, t + 1);
            }
        }
    };

    class BreakStmtAST final: public StmtAST {
    public:
        explicit BreakStmtAST(): StmtAST() { }

        void graphize(std::ostream& out, [[maybe_unused]] int t) override {
            out << "<break-stmt>\n";
        }
    };

    class ContinueStmtAST final: public StmtAST {
    public:
        explicit ContinueStmtAST(): StmtAST() { }

        void graphize(std::ostream& out, [[maybe_unused]] int t) override {
            out << "<continue-stmt>\n";
        }
    };
}

#endif //C0_JMP_STMT_H
