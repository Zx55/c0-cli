/*
 * Created by zx5 on 2019/11/28.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_VALUE_EXPR_H
#define C0_VALUE_EXPR_H

/* Contains left-value and right-value */

#include "basic_expr.h"

namespace cc0::ast {
    class Int32ExprAST final: public ExprAST {
    public:
        explicit Int32ExprAST(int32_t value): ExprAST(Type::INT, value) { }

        void graphize(std::ostream& out, [[maybe_unused]] int t) override {
            out << "<int32> [value] " << std::any_cast<int32_t>(_value) << "\n";
        }
    };

    class Float64ExprAST final: public ExprAST {
    public:
        explicit Float64ExprAST(double value): ExprAST(Type::DOUBLE, value) { }

        void graphize(std::ostream& out, [[maybe_unused]] int t) override {
            out << "<float64> [value] " << std::any_cast<double>(_value) << "\n";
        }
    };

    class CharExprAST final: public ExprAST {
    public:
        explicit CharExprAST(char value): ExprAST(Type::INT, static_cast<int32_t>(value)) { }

        void graphize(std::ostream& out, [[maybe_unused]] int t) override {
            int ch = std::any_cast<int32_t>(_value);
            out << "<char> [value] " << static_cast<char>(ch) << " (" << ch << ")\n";
        }
    };

    class StringExprAST final: public ExprAST {
    public:
        explicit StringExprAST(std::string value): ExprAST(Type::STRING, value) { }

        void graphize(std::ostream& out, [[maybe_unused]] int t) override {
            out << "<string> [value] " << std::any_cast<std::string>(_value) << "\n";
        }
    };

    class IdExprAST final: public ExprAST {
    private:
        Token _id;

    public:
        explicit IdExprAST(Token id): ExprAST(), _id(std::move(id)) { }

        void graphize(std::ostream& out, [[maybe_unused]] int t) override {
            out << "<id> [name] " << _id.get_value_string() << "\n";
        }
    };
}

#endif //C0_VALUE_EXPR_H
