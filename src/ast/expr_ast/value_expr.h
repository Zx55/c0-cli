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
        explicit Int32ExprAST(range_t range, int32_t value): ExprAST(range, Type::INT, value) { }

        void graphize(std::ostream& out, [[maybe_unused]] int t) override {
            out << "<int32> [value] " << std::any_cast<int32_t>(_value) << "\n";
        }

        [[nodiscard]] _GenResult generate([[maybe_unused]] _GenParam param) override {
            _gen_ist1(InstType::IPUSH, std::any_cast<int32_t>(_value));
            return _gen_ret(1);
        }
    };

    class Float64ExprAST final: public ExprAST {
    public:
        explicit Float64ExprAST(range_t range, double value): ExprAST(range, Type::DOUBLE, value) { }

        void graphize(std::ostream& out, [[maybe_unused]] int t) override {
            out << "<float64> [value] " << std::any_cast<double>(_value) << "\n";
        }

        [[nodiscard]] _GenResult generate([[maybe_unused]] _GenParam param) override {
            // store float in constant table
            auto value = std::any_cast<double>(_value);
            _symtbl.put_cons(Type::DOUBLE, value);
            auto offset = _symtbl.get_cons_offset(Type::DOUBLE, value);
            _gen_ist1(InstType::LOADC, offset);
            return _gen_ret(1);
        }
    };

    class CharExprAST final: public ExprAST {
    public:
        explicit CharExprAST(range_t range, char value): ExprAST(range, Type::INT, static_cast<int32_t>(value)) { }

        void graphize(std::ostream& out, [[maybe_unused]] int t) override {
            int ch = static_cast<char>(std::any_cast<int32_t>(_value));
            std::string output;

            switch (ch) {
                case '\r': output += "\\r"; break;
                case '\n': output += "\\n"; break;
                case '\t': output += "\\t"; break;
                default: output.append(1, ch);
            }

            out << "<char> [value] \'" << output << "\' (" << ch << ")\n";
        }

        [[nodiscard]] _GenResult generate([[maybe_unused]] _GenParam param) override {
            _gen_ist1(InstType::BIPUSH, std::any_cast<int32_t>(_value));
            return _gen_ret(1);
        }
    };

    class StringExprAST final: public ExprAST {
    public:
        explicit StringExprAST(range_t range, std::string value): ExprAST(range, Type::STRING, value) { }

        void graphize(std::ostream& out, [[maybe_unused]] int t) override {
            auto str = std::any_cast<std::string>(_value);
            replace_all(str, "\r", "\\r");
            replace_all(str, "\n", "\\n");
            replace_all(str, "\t", "\\t");
            out << "<string> [value] \"" << str << "\"\n";
        }

        [[nodiscard]] _GenResult generate([[maybe_unused]] _GenParam param) override {
            // store string in constant table
            auto str = std::any_cast<std::string>(_value);
            _symtbl.put_cons(Type::STRING, str);
            auto offset = _symtbl.get_cons_offset(Type::STRING, str);
            _gen_ist1(InstType::LOADC, offset);
            return _gen_ret(1);
        }
    };

    class IdExprAST final: public ExprAST {
    private:
        Token _id;

    public:
        explicit IdExprAST(range_t range, Token id): ExprAST(range), _id(std::move(id)) { }

        [[nodiscard]] inline Token get_id() const { return _id; }

        [[nodiscard]] inline std::string get_id_str() const { return _id.get_value_str(); }

        void graphize(std::ostream& out, [[maybe_unused]] int t) override {
            out << "<id> [name] " << _id.get_value_str() << "\n";
        }

        _GenResult generate(_GenParam param) override {

        }
    };
}

#endif //C0_VALUE_EXPR_H
