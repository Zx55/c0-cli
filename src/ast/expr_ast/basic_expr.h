/*
 * Created by zx5 on 2019/11/28.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_BASIC_EXPR_H
#define C0_BASIC_EXPR_H

#include "../basic_ast.h"

namespace cc0::ast {
    class ExprAST: public AST {
    protected:
        Type _type;

    public:
        explicit ExprAST(range_t range, Type type = Type::UNDEFINED):
             AST(range), _type(type) { }

        // we have to get expr's type after generating inst (parse its children)
        // like expression 1 + (2 * 3.0)
        // we don't know the type of 2 * 3.0 when we parse addition of 1 and (2 * 3.0)
        [[nodiscard]] inline Type get_type() const { return _type; }

        void graphize([[maybe_unused]] std::ostream& out, [[maybe_unused]] int t) override {
            out << "<expr>\n";
        }

        _GenResult generate([[maybe_unused]] _GenParam param) override {
            return { 0, {}, {} };
        }
    };
}

#endif //C0_BASIC_EXPR_H
