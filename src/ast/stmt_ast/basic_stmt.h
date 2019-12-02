/*
 * Created by zx5 on 2019/11/28.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_BASIC_STMT_H
#define C0_BASIC_STMT_H

#include "../basic_ast.h"
#include "../expr_ast/expr.h"

namespace cc0::ast {
    class StmtAST: public AST {
    public:
        explicit StmtAST(range_t range): AST(range) { }

        void graphize([[maybe_unused]] std::ostream& out, [[maybe_unused]] int t) override {
            out << "<stmt>\n";
        }
    };

    class EmptyStmtAST final: public StmtAST {
    public:
        explicit EmptyStmtAST(range_t range): StmtAST(range) { }

        void graphize(std::ostream& out, [[maybe_unused]] int t) override {
            out << "<empty-stmt>\n";
        }
    };
}

#endif //C0_BASIC_STMT_H
