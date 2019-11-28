/*
 * Created by zx5 on 2019/11/27.
 * Copyright (c) zx5. All rights reserved.
 */

#include "rdp.h"

#define _rdp_err(_type) C0Err(_type, _token.get_range())
#define _rdp_ptrs(_c) { std::make_move_iterator(_c.begin()), std::make_move_iterator(_c.end()) }

namespace cc0 {
    // <C0-program> ::= {<variable-declaration>}{<function-definition>}
    ast::_ptr<ast::RootAST> RDP::_analyse_root() {
        using namespace ast;

        auto vars = _ptrs<VarDeclAST>();
        auto funcs = _ptrs<FuncDefAST>();

        // {<variable-declaration>}
        while (true) {
            if (!_get())
                // EOF
                return std::make_unique<RootAST>(std::move(vars), std::move(funcs));

            if (_token.get_type() == TokenType::CONST) {
                _unget();
                auto res = _analyse_var_decl();
                vars.insert(vars.end(), std::make_move_iterator(res.begin()),
                            std::make_move_iterator(res.end()));
            } else if (istype(_token.get_type())) {
                if (!_get()) {
                    _errs.emplace_back(_rdp_err(ErrCode::ErrMissIdentifier));
                    return std::make_unique<RootAST>(std::move(vars), std::move(funcs));
                }

                if (_token.get_type() != TokenType::IDENTIFIER) {
                    _errs.emplace_back(_rdp_err(ErrCode::ErrMissIdentifier));
                    continue;
                }

                if (!_get()) {
                    _errs.emplace_back(_rdp_err(ErrCode::ErrMissSemi));
                    return std::make_unique<RootAST>(std::move(vars), std::move(funcs));
                }

                // read <function-definition>
                if (_token.get_type() == TokenType::LPARENTHESIS) {
                    _unget(); _unget(); _unget();
                    break;
                }

                _unget(); _unget(); _unget();
                auto res = _analyse_var_decl();
                vars.insert(vars.end(), std::make_move_iterator(res.begin()),
                            std::make_move_iterator(res.end()));
            }
        }

        // {<function-definition>}
        while (true) {
            if (!_get())
                // EOF
                return std::make_unique<RootAST>(std::move(vars), std::move(funcs));

            _unget();
            if (auto res = _analyse_func_def(); res)
                funcs.push_back(std::move(res));
        }
    }

    // <variable-declaration> ::= [<const>]<type><init-decl-list>';'
    ast::_ptrs<ast::VarDeclAST> RDP::_analyse_var_decl() {
        using namespace ast;

        auto vars = _ptrs<VarDeclAST>();
        bool f_const = false;

        (void) _get();
        // [<const>]
        if (_token.get_type() == TokenType::CONST) {
            f_const = true;

            if (!_get()) {
                _errs.emplace_back(_rdp_err(ErrCode::ErrMissTypeSpecifier));
                return { };
            }
        }

        // <type>
        Type type = _analyse_type_specifier();
        if (type == Type::UNDEFINED) return { };

        // <init-decl-list> ::= <init-decl>{','<init-decl>}
        // <init-decl>      ::= <identifier>['='<expression>]
        while (true) {
            // <identifier>
            auto id = _analyser_id();
            if (!id) return _rdp_ptrs(vars);

            if (!_get()) {
                _errs.emplace_back(_rdp_err(ErrCode::ErrMissSemi));
                return _rdp_ptrs(vars);
            }
            // ['='<expression>]
            if (_token.get_type() == TokenType::EQUAL) {
                if (auto res = _analyse_expr(); res)
                    vars.push_back(std::make_unique<VarDeclAST>(type, std::move(id), std::move(res), f_const));
            } else
                _unget();

            if (!_get()) {
                _errs.emplace_back(_rdp_err(ErrCode::ErrMissSemi));
                return _rdp_ptrs(vars);
            }
            // {','<init-decl>}
            if (_token.get_type() == TokenType::SEMI)
                break;
            else if (_token.get_type() == TokenType::COMMA)
                vars.push_back(std::make_unique<VarDeclAST>(type, std::move(id), nullptr, f_const));
            else {
                _errs.emplace_back(_rdp_err(ErrCode::ErrInvalidDeclaration));
                return _rdp_ptrs(vars);
            }
        }

        // already read ';'
        return _rdp_ptrs(vars);
    }

    // <function-definition> ::= <type><identifier><params><block-stmts>
    ast::_ptr<ast::FuncDefAST> RDP::_analyse_func_def() {
        using namespace ast;

        (void) _get();
        // <type>
        Type type = _analyse_type_specifier();
        if (type == Type::UNDEFINED) return nullptr;

        // <identifier>
        auto id = _analyser_id();
        if (!id) return nullptr;

        // <params> ::= '('[<params-decl-list>]')'
        if (!_get() || _token.get_type() != TokenType::LPARENTHESIS) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissParenthesis));
            return nullptr;
        }
        auto params = _analyse_params();
        if (!_get() || _token.get_type() != TokenType::RPARENTHESIS) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissParenthesis));
            return nullptr;
        }

        // <block-stmts>
        auto block = _analyse_block();
        if (!block) return nullptr;

        return std::make_unique<FuncDefAST>(type, std::move(id), std::move(params), std::move(block));
    }

    ast::_ptrs<ast::ParamAST> RDP::_analyse_params() {
        return { };
    }

    ast::_ptr<ast::BlockStmtAST> RDP::_analyse_block() {
        return nullptr;
    }

    // <type>
    // must pre-read
    inline ast::Type RDP::_analyse_type_specifier() {
        using namespace ast;

        Type type = make_type(_token.get_type());
        if (type == Type::UNDEFINED) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrInvalidTypeSpecifier));
            return Type::UNDEFINED;
        }

        return type;
    }

    ast::_ptr<ast::ExprAST> RDP::_analyse_expr() {
        using namespace ast;

        return nullptr;
    }

    inline ast::_ptr<ast::IdExprAST> RDP::_analyser_id() {
        using namespace ast;

        if (!_get() || _token.get_type() != TokenType::IDENTIFIER) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissIdentifier));
            return nullptr;
        }

        return std::make_unique<IdExprAST>(_token);
    }

    void RDP::analyse() {
        using namespace ast;

        RuntimeContext::set_ast(_analyse_root());
        RuntimeContext::put_fatals(std::move(_errs));
        RuntimeContext::put_wrns(std::move(_wrns));
    }
}
