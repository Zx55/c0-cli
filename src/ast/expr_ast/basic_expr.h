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
        std::any _value;

    public:
        explicit ExprAST(Type type = Type::UNDEFINED, std::any value = std::any(0)):
            _type(type), _value(std::move(value)) { }

        void graphize([[maybe_unused]] std::ostream& out, [[maybe_unused]] int t) override {
            out << "<expr>\n";
        }
    };
}

#endif //C0_BASIC_EXPR_H
