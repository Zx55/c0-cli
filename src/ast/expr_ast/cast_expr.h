/*
 * Created by zx5 on 2019/11/29.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_CAST_EXPR_H
#define C0_CAST_EXPR_H

#include "basic_expr.h"

namespace cc0::ast {
    class CastExprAST final: public ExprAST {
    private:
        Type _cast;
        _ptr<ExprAST> _expr;

    public:
        explicit CastExprAST(range_t range, Type cast, _ptr<ExprAST> expr):
            ExprAST(range), _cast(cast), _expr(std::move(expr)) { }

        [[nodiscard]] inline Type get_type() override {
            return _cast;
        }

        void graphize(std::ostream& out, int t) override {
            out << "<cast-expr> [type] " << _type_str(_cast) << "\n" << _end(t);
            _expr->graphize(out, t + 1);
        }

        [[nodiscard]] _GenResult generate(_GenParam param) override {
            auto src_type = _expr->get_type();
            if (src_type == Type::UNDEFINED)
                return _gen_ret(0);
            if (_cast == Type::VOID || src_type == Type::VOID) {
                // we don't allow void => any and any => void
                _gen_err(ErrCode::ErrVoidHasNoValue);
                return _gen_ret(0);
            }

            auto res = _expr->generate(param);
            auto len = res._len;
            if (len == 0)
                return _gen_ret(0);

            if (_cast == src_type) {
                ExprAST::_type = _cast;
                return _gen_ret(len);
            }

            switch (src_type) {
                case Type::DOUBLE:
                    _gen_ist0(InstType::D2I);
                    if (_cast == Type::CHAR) {
                        _gen_ist0(InstType::I2C);
                        ++len;
                    }
                    ++len;
                    break;
                case Type::INT:
                    if (_cast == Type::DOUBLE)
                        _gen_ist0(InstType::I2D);
                    else if (_cast == Type::CHAR)
                        _gen_ist0(InstType::I2C);
                    ++len;
                    break;
                case Type::CHAR:
                    if (_cast == Type::DOUBLE) {
                        _gen_ist0(InstType::I2D);
                        ++len;
                    }
                    break;
                default:
                    _gen_popn(len);
                    return _gen_ret(0);
            }

            return _gen_ret(len);
        }
    };
}

#endif //C0_CAST_EXPR_H
