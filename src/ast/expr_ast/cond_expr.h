/*
 * Created by zx5 on 2019/11/28.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_COND_EXPR_H
#define C0_COND_EXPR_H

#include "basic_expr.h"
#include "value_expr.h"

namespace cc0::ast {
    class CondExprAST final: public ExprAST {
    private:
        _ptr<ExprAST> _lhs;
        Op _op;
        _ptr<ExprAST> _rhs;

        [[nodiscard]] inline static InstType _make_cmp(Type type) {
            if (type == Type::DOUBLE)
                return InstType::DCMP;
            return InstType::ICMP;
        }

    public:
        explicit CondExprAST(range_t range, _ptr<ExprAST> lhs):
                ExprAST(range, Type::INT), _lhs(std::move(lhs)), _op(Op::NEQ) {
            _rhs = std::make_unique<Int32ExprAST>(range, 0);
        }
        explicit CondExprAST(range_t range, _ptr<ExprAST> lhs, Op op, _ptr<ExprAST> rhs):
            ExprAST(range, Type::INT), _lhs(std::move(lhs)), _op(op), _rhs(std::move(rhs)) { }

        [[nodiscard]] inline Op get_op() const { return _op; }

        void graphize(std::ostream& out, int t) override {
            out << "<cond-expr> [op] " << _op_str(_op) << "\n" << _mid(t);
            _lhs->graphize(out, t + 1);
            out << _end(t);
            _rhs->graphize(out, t + 1);
        }

        [[nodiscard]] _GenResult generate(_GenParam param) override {
            uint32_t len = 0;

            auto lres = _lhs->generate(param);
            len += lres._len;
            auto ltype = _lhs->get_type();

            if (ltype == Type::VOID || ltype == Type::STRING) {
                _gen_err(ErrCode::ErrIncomparable);
                _gen_popn(len);
                return _gen_ret(0);
            }

            // generate inst first
            auto rres = _rhs->generate(param);
            len += rres._len;
            auto rtype = _rhs->get_type();

            if (rtype == Type::VOID || rtype == Type::STRING) {
                _gen_err(ErrCode::ErrIncomparable);
                _gen_popn(len);
                return _gen_ret(0);
            }

            if (ltype == rtype) {
                _gen_ist0(_make_cmp(ltype));
                return _gen_ret(len + 1);
            }

            // double vs int/char
            if (ltype == Type::DOUBLE) {
                _gen_ist0(InstType::I2D);
                _gen_ist0(_make_cmp(ltype));
                return _gen_ret(len + 2);
            }

            if (rtype == Type::DOUBLE) {
                // regenerate rhs
                _gen_popn(rres._len);
                _gen_ist0(InstType::I2D);
                (void) _rhs->generate(param);
                _gen_ist0(_make_cmp(rtype));
                return _gen_ret(len + 2);
            }

            // int vs char
            _gen_ist0(InstType::ICMP);
            return _gen_ret(len + 1);
        }
    };
}

#endif //C0_COND_EXPR_H
