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

        ~StmtAST() override = default;

        void graphize([[maybe_unused]] std::ostream& out, [[maybe_unused]] int t) override {
            out << "<stmt>\n";
        }

        _GenResult generate([[maybe_unused]] _GenParam param) override {
            return { 0, {}, {} };
        }
    };

    class EmptyStmtAST final: public StmtAST {
    public:
        explicit EmptyStmtAST(range_t range): StmtAST(range) { }

        void graphize(std::ostream& out, [[maybe_unused]] int t) override {
            out << "<empty-stmt>\n";
        }

        [[nodiscard]] _GenResult generate([[maybe_unused]] _GenParam param) override {
            _gen_ist0(InstType::NOP);
            return _gen_ret(1);
        }
    };
}

#endif //C0_BASIC_STMT_H
