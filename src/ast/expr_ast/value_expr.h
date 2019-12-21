/*
 * Created by zx5 on 2019/11/28.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_VALUE_EXPR_H
#define C0_VALUE_EXPR_H

/* Contains left-value and right-value */

#include <utility>

#include "basic_expr.h"

namespace cc0::ast {
    class Int32ExprAST final: public ExprAST {
    private:
        int32_t _value;

    public:
        explicit Int32ExprAST(range_t range, int32_t value): ExprAST(range, Type::INT), _value(value) { }

        [[nodiscard]] inline Type get_type() override {
            return _type;
        }

        [[nodiscard]] inline int32_t get_value() const override {
            return _value;
        }

        void graphize(std::ostream& out, [[maybe_unused]] int t) override {
            out << "<int32> [value] " << _value << "\n";
        }

        [[nodiscard]] _GenResult generate([[maybe_unused]] _GenParam param) override {
            _gen_ist1(InstType::IPUSH, _value);
            return _gen_ret(1);
        }
    };

    class Float64ExprAST final: public ExprAST {
    private:
        double _value;

    public:
        explicit Float64ExprAST(range_t range, double value): ExprAST(range, Type::DOUBLE), _value(value) { }

        [[nodiscard]] inline Type get_type() override {
            return _type;
        }

        void graphize(std::ostream& out, [[maybe_unused]] int t) override {
            out << "<float64> [value] " << _value << "\n";
        }

        [[nodiscard]] _GenResult generate([[maybe_unused]] _GenParam param) override {
            // store float in constant table
            _symtbl.put_cons(Type::DOUBLE, _value);
            auto offset = _symtbl.get_cons_offset(Type::DOUBLE, _value);
            _gen_ist1(InstType::LOADC, offset);
            return _gen_ret(1);
        }
    };

    class CharExprAST final: public ExprAST {
    private:
        int32_t _value;

    public:
        explicit CharExprAST(range_t range, char value): ExprAST(range, Type::INT),
            _value(static_cast<int32_t>(value)) { }

        [[nodiscard]] inline Type get_type() override {
            return _type;
        }

        [[nodiscard]] inline int32_t get_value() const override {
            return _value;
        }

        [[nodiscard]] inline Type get_real_type() override {
            return Type::CHAR;
        }

        void graphize(std::ostream& out, [[maybe_unused]] int t) override {
            int ch = static_cast<char>(_value);
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
            _gen_ist1(InstType::BIPUSH, _value);
            return _gen_ret(1);
        }
    };

    class StringExprAST final: public ExprAST {
    private:
        std::string _value;

    public:
        explicit StringExprAST(range_t range, std::string value):
            ExprAST(range, Type::STRING), _value(std::move(value)) { }

        [[nodiscard]] inline Type get_type() override {
            return _type;
        }

        void graphize(std::ostream& out, [[maybe_unused]] int t) override {
            auto str = _value;
            replace_all(str, "\r", "\\r");
            replace_all(str, "\n", "\\n");
            replace_all(str, "\t", "\\t");
            out << "<string> [value] \"" << str << "\"\n";
        }

        [[nodiscard]] _GenResult generate([[maybe_unused]] _GenParam param) override {
            // store string in constant table
            _symtbl.put_cons(Type::STRING, _value);
            auto offset = _symtbl.get_cons_offset(Type::STRING, _value);
            _gen_ist1(InstType::LOADC, offset);
            return _gen_ret(1);
        }
    };

    class IdExprAST final: public ExprAST {
    private:
        Token _id;

        [[nodiscard]] inline static InstType _make_load(Type type) {
            if (type == Type::DOUBLE)
                return InstType::DLOAD;
            return InstType::ILOAD;
        }

    public:
        explicit IdExprAST(range_t range, Token id): ExprAST(range), _id(std::move(id)) { }

        [[nodiscard]] inline Token get_id() const { return _id; }

        [[nodiscard]] inline std::string get_id_str() const { return _id.get_value_str(); }

        [[nodiscard]] inline Type get_type() override {
            if (_type != Type::UNDEFINED) return _type;

            if (auto func = _symtbl.get_func(get_id_str()); func.has_value()) {
                _gen_err(ErrCode::ErrFuncIdHasNoValue);
                return _type;
            }

            auto var = _symtbl.get_var(get_id_str());
            if (!var.has_value()) {
                _gen_err(ErrCode::ErrUndeclaredIdentifier);
                return _type;
            }

            ExprAST::_type = var->get_type();
            return _type;
        }

        void graphize(std::ostream& out, [[maybe_unused]] int t) override {
            out << "<id> [name] " << _id.get_value_str() << "\n";
        }

        [[nodiscard]] _GenResult generate(_GenParam param) override {
            // get variable address only
            auto var = _symtbl.get_var(get_id_str());

            if (var->is_glob())
                _gen_ist2(InstType::LOADA, 1, var->get_offset());
            else
                _gen_ist2(InstType::LOADA, 0, var->get_offset());

            // we use ret field to check whether storing or loading
            // UNDEFINED => store, like scan and assignment
            // otherwise => load, like print, add
            if (param._ret != Type::UNDEFINED) {
                if (!var->is_init())
                    _gen_wrn(ErrCode::WrnUninitializedVariable);

                _gen_ist0(_make_load(var->get_type()));
                return _gen_ret(2);
            }

            return _gen_ret(1);
        }
    };
}

#endif //C0_VALUE_EXPR_H
