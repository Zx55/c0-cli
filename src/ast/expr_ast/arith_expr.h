/*
 * Created by zx5 on 2019/11/28.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_ARITH_EXPR_H
#define C0_ARITH_EXPR_H

#include "basic_expr.h"

namespace cc0::ast {
    class BinaryExprAST final: public ExprAST {
    private:
        _ptr<ExprAST> _lhs;
        Op _op;
        _ptr<ExprAST> _rhs;

    public:
        explicit BinaryExprAST(range_t range, _ptr<ExprAST> lhs, Op op, _ptr<ExprAST> rhs):
            ExprAST(range), _lhs(std::move(lhs)), _op(op), _rhs(std::move(rhs)) { }

        [[nodiscard]] inline Type get_type() override {
            if (_type != Type::UNDEFINED) return _type;

            auto ltype = _lhs->get_type(), rtype = _rhs->get_type();
            // error
            if (ltype == Type::UNDEFINED || rtype == Type::UNDEFINED) return _type;
            if (ltype == Type::VOID || rtype == Type::VOID) {
                _gen_err(ErrCode::ErrVoidHasNoValue);
                return _type;
            }

            if (ltype == Type::DOUBLE || rtype == Type::DOUBLE)
                _type = Type::DOUBLE;
            else if (ltype == rtype)
                _type = ltype;
            else
                _type = Type::INT;
            return _type;
        }

        void graphize(std::ostream& out, int t) override {
            out << "<binary-expr> [op] " << _op_str(_op) << "\n" << _mid(t);
            _lhs->graphize(out, t + 1);
            out << _end(t);
            _rhs->graphize(out, t + 1);
        }

        _GenResult generate(_GenParam param) override {

        }
    };

    class UnaryExprAST final: public ExprAST {
    private:
        bool _sign; // true => '+'; false => '-'
        _ptr<ExprAST> _expr;

        [[nodiscard]] inline static InstType _make_neg(Type type) {
            if (type == Type::DOUBLE)
                return InstType::DNEG;
            return InstType::INEG;
        }

    public:
        explicit UnaryExprAST(range_t range, bool sign, _ptr<ExprAST> expr):
            ExprAST(range), _sign(sign), _expr(std::move(expr)) { }

        [[nodiscard]] inline Type get_type() override {
            if (_type == Type::UNDEFINED)
                ExprAST::_type = _expr->get_type();
            return _type;
        }

        void graphize(std::ostream& out, int t) override {
            out << "<unary-expr> [sign] " << (_sign ? '+' : '-') << "\n" << _end(t);
            _expr->graphize(out, t + 1);
        }

        [[nodiscard]] _GenResult generate(_GenParam param) override {
            auto type = _expr->get_type();
            if (type == Type::UNDEFINED)
                return _gen_ret(0);
            if (type == Type::VOID) {
                _gen_err(ErrCode::ErrVoidHasNoValue);
                return _gen_ret(0);
            }

            auto res = _expr->generate(param);
            if (res._len == 0)
                return _gen_ret(0);

            _gen_ist0(_make_neg(type));
            return _gen_ret(res._len + 1);
        }
    };
}

#endif //C0_ARITH_EXPR_H
