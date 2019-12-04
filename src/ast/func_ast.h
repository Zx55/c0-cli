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
                return _gen_ret(0);
            _gen_err(ErrCode::ErrRedeclaredIdentifier);
            return _gen_ret(1);
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

        _GenResult generate(_GenParam param) override {

        }
    };

    class FuncDefAST final: public AST {
    private:
        Type _ret;
        _ptr<IdExprAST> _id;
        _ptrs<ParamAST> _params;
        _ptr<BlockStmtAST> _block;

    public:
        explicit FuncDefAST(range_t range, Type ret, _ptr<IdExprAST> id,
                _ptrs<ParamAST> params, _ptr<BlockStmtAST> block):
            AST(range), _ret(ret), _id(std::move(id)), _params(std::move(params)), _block(std::move(block)) { }

        [[nodiscard]] inline Type get_ret() const { return _ret; }

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
            _symtbl.put_func(_id->get_id_str(), _ret, param._offset);

            // parameters
            uint32_t slot = 0;
            for (const auto &p: _params) {
                if (auto res = p->generate({param._level, 0, 0, param._ret}); res._len == 0)
                    slot += _make_slot(p->get_type());
            }

            auto res = _block->generate({param._level, 0, slot, _ret});
            // check breaks and continues
            if (!res._breaks.empty() || !res._continues.empty()) {
                _gen_err(ErrCode::ErrJmpInAcyclicStmt);
                _gen_popn(res._len);
                return _gen_ret(0);
            }
            return _gen_ret(res._len);
        }
    };

    class FuncCallAST final: public ExprAST, public StmtAST {
    private:
        _ptr<IdExprAST> _id;
        _ptrs<ExprAST> _params;
        range_t _range;

    public:
        explicit FuncCallAST(range_t range, _ptr<IdExprAST> id, _ptrs<ExprAST> params):
            ExprAST(range), StmtAST(range), _id(std::move(id)), _params(std::move(params)), _range(range) { }

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

            // check undeclared
            if (!func.has_value()) {
                _gen_err(ErrCode::ErrUndeclaredIdentifier);
                return _gen_ret(0);
            }

            // check param
            auto params = func->get_params();
            if (params.size() != _params.size()) {
                _gen_err(ErrCode::ErrParameterUnMatch);
                return _gen_ret(0);
            }
            // push param
            auto it1 = params.cbegin();
            auto it2 = _params.cbegin();
            for (; it1 != params.cend(); ++it1, ++it2) {
                (*it2)->generate(param);
                // type check

            }
        }
    };
}

#endif //C0_FUNC_AST_H
