/*
 * Created by zx5 on 2019/11/27.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_VAR_AST_H
#define C0_VAR_AST_H

#include "basic_ast.h"
#include "expr_ast/expr.h"

#define _put_tbl(_flg) ((_glob) ? (_symtbl.put_glob(_id->get_id_str(), _type, \
    param._slot, param._level, _flg, _const)) : (_symtbl.put_local(_id->get_id_str(), \
    _type, param._slot, param._level, _flg, _const)))

namespace cc0::ast {
    class VarDeclAST final: public AST {
    private:
        ast::Type _type;
        _ptr<IdExprAST> _id;
        _ptr<ExprAST> _init;
        bool _const;
        bool _glob;

        [[nodiscard]] _GenResult _gen_var_decl(const _GenParam& param) {
            // uninitialized
            if (_init == nullptr) {
                _gen_ist1(InstType::SNEW, _make_slot(_type));
                _put_tbl(false);

                if (_const) _gen_wrn(ErrCode::WrnUninitailizedConstant);
                return _gen_ret(1);
            }

            // initialized
            auto len = _init->generate(param)._len;
            if (len == 0)
                return _gen_ret(0);

            auto init_type = _init->get_type();
            if (init_type == _type) {
                _put_tbl(true);
                return _gen_ret(len);
            }

            switch (init_type) {
                case Type::VOID: {
                    _gen_err(ErrCode::ErrVoidHasNoValue);
                    _gen_pop;
                    return _gen_ret(0);
                }
                case Type::DOUBLE: {
                    // int a = 1.0;      perform double => int
                    _gen_ist0(InstType::D2I);
                    _put_tbl(true);
                    return _gen_ret(len + 1);
                }
                case Type::INT: {
                    if (_type == Type::DOUBLE)
                        // double a = 1; perform int => double
                        _gen_ist0(InstType::I2D);
                    else if (_type == Type::CHAR)
                        // char c = 1;   perform int => char
                        _gen_ist0(InstType::I2C);
                    _put_tbl(true);
                    return _gen_ret(len + 1);
                }
                case Type::CHAR: {
                    _put_tbl(true);
                    if (_type == Type::DOUBLE) {
                        // double 1 = 'c';
                        _gen_ist0(InstType::I2D);
                        return _gen_ret(len + 1);
                    }
                    return _gen_ret(len);
                }
                default:
                    return _gen_ret(0);
            }
        }

    public:
        explicit VarDeclAST(range_t range, Type type, _ptr<IdExprAST> id, _ptr<ExprAST> init,
                bool f_const = false, bool glob = false):
            AST(range), _type(type), _id(std::move(id)), _init(std::move(init)), _const(f_const), _glob(glob) { }

        [[nodiscard]] inline Type get_type() const { return _type; }

        void graphize(std::ostream& out, int t) override {
            out << "<var-decl> [type] " << (_const ? "const " : "") << _type_str(_type) << "\n";
            out << (_init == nullptr ? _end(t) : _mid(t));
            _id->graphize(out, t + 1);
            if (_init != nullptr) {
                out << _end(t);
                _init->graphize(out, t + 1);
            }
        }

        [[nodiscard]] _GenResult generate(_GenParam param) override {
            if (_type == Type::VOID) {
                _gen_err(ErrCode::ErrInvalidTypeSpecifier);
                return _gen_ret(0);
            }

            // check redeclaration
            auto var = _glob ? _symtbl.get_var(_id->get_id_str()) :
                    _symtbl.get_var(_id->get_id_str(), param._level);
            if (var.has_value()) {
                _gen_err(ErrCode::ErrRedeclaredIdentifier);
                return _gen_ret(0);
            }

            return _gen_var_decl(param);
        }
    };
}

#endif //C0_VAR_AST_H
