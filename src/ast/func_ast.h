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

        void graphize(std::ostream& out, int t) override {
            out << "[type] " << (_const ? "const " : "") << _type_str(_type) << " ";
            _id->graphize(out, t + 1);
        }

        _GenResult generate(_GenParam param) override {

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

        _GenResult generate(_GenParam param) override {

        }
    };

    class FuncCallAST final: public ExprAST, public StmtAST {
    private:
        _ptr<IdExprAST> _id;
        _ptrs<ExprAST> _params;

    public:
        explicit FuncCallAST(range_t range, _ptr<IdExprAST> id, _ptrs<ExprAST> params):
            ExprAST(range), StmtAST(range), _id(std::move(id)), _params(std::move(params)) { }

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

        _GenResult generate(_GenParam param) override {

        }
    };
}

#endif //C0_FUNC_AST_H
