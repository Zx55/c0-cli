/*
 * Created by zx5 on 2019/11/28.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_FUNC_AST_H
#define C0_FUNC_AST_H

#include "basic_ast.h"
#include "var_ast.h"
#include "expr_ast/expr.h"
#include "stmt_ast/stmt.h"

namespace cc0::ast {
    class ParamAST final: public AST {
    private:
        std::string _func;
        Type _type;
        _ptr<IdExprAST> _id;
        bool _const;

    public:
        explicit ParamAST(range_t range, std::string func, Type type, _ptr<IdExprAST> id, bool f_const = false):
            AST(range), _func(std::move(func)), _type(type), _id(std::move(id)), _const(f_const) { }

        [[nodiscard]] inline Type get_type() const { return _type; }

        void graphize(std::ostream& out, int t) override {
            out << "[type] " << (_const ? "const " : "") << _type_str(_type) << " ";
            _id->graphize(out, t + 1);
        }

        [[nodiscard]] _GenResult generate(_GenParam param) override {
            if (_type == Type::VOID || _type == Type::UNDEFINED) {
                _gen_err(ErrCode::ErrInvalidTypeSpecifier);
                return _gen_ret(1);
            }

            _symtbl.put_local(_id->get_id_str(), _type, param._slot, param._level, true, _const);
            if (_symtbl.put_param(_func, _id->get_id_str(), _type, _const))
                return _gen_ret(1);
            _gen_err(ErrCode::ErrRedeclaredIdentifier);
            return _gen_ret(0);
        }
    };

    class BlockStmtAST final: public StmtAST {
    private:
        _ptrs<VarDeclAST> _vars;
        _ptrs<StmtAST> _stmts;

    public:
        explicit BlockStmtAST(range_t range, _ptrs<VarDeclAST> vars, _ptrs<StmtAST> stmts):
            StmtAST(range), _vars(std::move(vars)), _stmts(std::move(stmts)) { }

        void graphize(std::ostream& out, int t) override {
            out << "<block>\n";

            if (!_vars.empty()) {
                for (auto it = _vars.cbegin(); it != _vars.cend() - 1; ++it) {
                    out << _mid(t);
                    (*it)->graphize(out, t + 1);
                }
                if (_stmts.empty()) out << _end(t);
                else out << _mid(t);
                (*(_vars.cend() - 1))->graphize(out, t + 1);
            }

            _graphize_list(_stmts, out, t, t + 1);
        }

        [[nodiscard]] _GenResult generate(_GenParam param) override {
            auto slot = param._slot;
            uint32_t len = 0;
            std::vector<uint32_t> breaks, continues;

            for (const auto& var: _vars) {
                auto res = var->generate({ param._level + 1, param._offset,
                                           slot, param._ret });
                if (res._len != 0) slot += _make_slot(var->get_type());
                len += res._len;
            }

            for (const auto& stmt: _stmts) {
                auto res = stmt->generate({ param._level + 1, param._offset + len,
                                            slot, param._ret});
                len += res._len;
                _gen_move_back(breaks, res._breaks);
                _gen_move_back(continues, res._continues);
            }

            _symtbl.destroy_level(param._level + 1);
            return { len, std::move(breaks), std::move(continues) };
        }
    };

    class FuncDefAST final: public AST {
    private:
        Type _ret;
        _ptr<IdExprAST> _id;
        _ptrs<ParamAST> _params;
        _ptr<BlockStmtAST> _block;

        [[nodiscard]] inline uint32_t _make_default_ret() {
            switch (_ret) {
                case Type::INT:
                    [[fallthrough]];
                case Type::CHAR: {
                    _gen_ist1(InstType::IPUSH, -1);
                    _gen_ist0(InstType::IRET);
                    return 2;
                }
                case Type::DOUBLE: {
                    _symtbl.put_cons(Type::DOUBLE, -1.0);
                    auto offset = _symtbl.get_cons_offset(Type::DOUBLE, -1.0);
                    _gen_ist1(InstType::LOADC, offset);
                    _gen_ist0(InstType::DRET);
                    return 2;
                }
                default: {
                    _gen_ist0(InstType::RET);
                    return 1;
                }
            }
        }

    public:
        explicit FuncDefAST(range_t range, Type ret, _ptr<IdExprAST> id,
                _ptrs<ParamAST> params, _ptr<BlockStmtAST> block):
            AST(range), _ret(ret), _id(std::move(id)), _params(std::move(params)), _block(std::move(block)) { }

        [[nodiscard]] inline Type get_ret() const { return _ret; }

        [[nodiscard]] inline std::string get_id_str() const { return _id->get_id_str(); }

        void graphize(std::ostream &out, int t) override {
            out << "<func-def> [return] -> " << _type_str(_ret) << "\n" << _mid(t);
            _id->graphize(out, t + 1);
            if (!_params.empty()) out << _mid(t) << "<params>\n";
            _graphize_list(_params, out, t + 1, t + 1);
            out << _end(t);
            _block->graphize(out, t + 1);
        }

        [[nodiscard]] _GenResult generate(_GenParam param) override {
            _symtbl.clear_local();

            // redeclaration
            if (_symtbl.get_var(_id->get_id_str()).has_value()
                || _symtbl.get_func(_id->get_id_str()).has_value()) {
                _gen_err(ErrCode::ErrRedeclaredIdentifier);
                return _gen_ret(0);
            }

            _symtbl.put_cons(Type::STRING, _id->get_id_str());
            _symtbl.put_local(_id->get_id_str(), _ret, -1, param._level);
            _symtbl.put_func(_id->get_id_str(), _ret, _gen_ist_off);

            // parameters
            uint32_t slot = 0;
            for (const auto &p: _params) {
                if (auto res = p->generate({ 1, 0, slot, param._ret}); res._len != 0)
                    slot += _make_slot(p->get_type());
            }

            // block will generate first code, so _offset must be 0
            auto res = _block->generate({ 0, 0, slot, _ret });
            if (res._len == 0)
                return _gen_ret(0);

            // check breaks and continues
            if (!res._breaks.empty() || !res._continues.empty()) {
                // FIXME: make correct error pos of break
                _gen_err(ErrCode::ErrJmpInAcyclicStmt);
                _gen_popn(res._len);
                return _gen_ret(0);
            }

            // for those branch lack of return
            auto len = _make_default_ret();

            return _gen_ret(res._len + len);
        }
    };

    class FuncCallAST final: public ExprAST, public StmtAST {
    private:
        _ptr<IdExprAST> _id;
        _ptrs<ExprAST> _params;

    public:
        explicit FuncCallAST(range_t range, _ptr<IdExprAST> id, _ptrs<ExprAST> params):
            ExprAST(range), StmtAST(range), _id(std::move(id)), _params(std::move(params)) { }

        [[nodiscard]] inline Type get_type() override {
            if (_type != Type::UNDEFINED) return _type;

            // check undeclared
            auto func = _symtbl.get_func(_id->get_id_str());
            if (!func.has_value()) {
                _gen_err2(ErrCode::ErrUndeclaredIdentifier);
                return Type::UNDEFINED;
            }

            ExprAST::_type = func->get_ret_type();
            return _type;
        }

        void graphize(std::ostream &out, int t) override {
            out << "<func-call> ";
            _id->graphize(out, t + 1);
            if (!_params.empty()) {
                for (auto it = _params.cbegin(); it != _params.cend() - 1; ++it) {
                    out << _mid(t);
                    (*it)->graphize(out, t + 1);
                }
                out << _end(t);
                (*(_params.cend() - 1))->graphize(out, t + 1);
            }
        }

        [[nodiscard]] _GenResult generate(_GenParam param) override {
            auto func = _symtbl.get_func(_id->get_id_str());

            // we check func in get_type
            auto params = func->get_params();
            if (params.size() != _params.size()) {
                _gen_err2(ErrCode::ErrParameterUnMatch);
                return _gen_ret(0);
            }

            uint32_t len = 0;
            auto it_formal = params.cbegin();
            auto it_actual = _params.cbegin();
            for (; it_formal != params.cend(); ++it_formal, ++it_actual) {
                auto actual_type = (*it_actual)->get_type(), formal_type = it_formal->second.first;
                // check type
                if (actual_type == Type::UNDEFINED) {
                    _gen_popn(len);
                    return _gen_ret(0);
                }
                if (actual_type == Type::VOID) {
                    _gen_err2(ErrCode::ErrVoidHasNoValue);
                    _gen_popn(len);
                    return _gen_ret(0);
                }

                // push actual param
                auto res = (*it_actual)->generate(param);
                if (res._len == 0) {
                    _gen_popn(len);
                    return _gen_ret(0);
                }
                len += res._len;

                // type trans
                if (formal_type == actual_type)
                    continue;
                switch (actual_type) {
                    case Type::DOUBLE:
                        // foo(int), foo(1.0);     perform double => int
                        _gen_ist0(InstType::D2I);
                        ++len;
                        break;
                    case Type::INT:
                        if (formal_type == Type::DOUBLE)
                            // foo(double), foo(1); perform int => double
                            _gen_ist0(InstType::I2D);
                        else if (formal_type == Type::CHAR)
                            // foo(char), foo(1);   perform int => char
                            _gen_ist0(InstType::I2C);
                        ++len;
                        break;
                    case Type::CHAR:
                        if (formal_type == Type::DOUBLE) {
                            // foo(double), foo('c'); perform char => double
                            _gen_ist0(InstType::I2D);
                            ++len;
                        }
                        break;
                    default:
                        _gen_popn(len);
                        return _gen_ret(0);
                }
            }

            _gen_ist0(InstType::CALL);
            return _gen_ret(len + 1);
        }
    };
}

#endif //C0_FUNC_AST_H
