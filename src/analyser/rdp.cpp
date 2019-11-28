/*
 * Created by zx5 on 2019/11/27.
 * Copyright (c) zx5. All rights reserved.
 */

#include "rdp.h"

#define _rdp_err(_type) C0Err(_type, _token.get_range())
#define _rdp_ptrs(_c) { std::make_move_iterator(_c.begin()), std::make_move_iterator(_c.end()) }
#define _rdp_move_back(_src, _res) _src.insert(_src.end(), std::make_move_iterator(_res.begin()), \
    std::make_move_iterator(_res.end()))
#define _rdp_stmt(_res) std::unique_ptr<StmtAST>(std::move(_res))

namespace cc0 {
    // <C0> ::= {<var-decl>}{<func-def>}
    ast::_ptr<ast::RootAST> RDP::_analyse_root() {
        using namespace ast;

        auto vars = _ptrs<VarDeclAST>();
        auto funcs = _ptrs<FuncDefAST>();

        // {<variable-declaration>}
        while (true) {
            if (!_get())
                // EOF
                return std::make_unique<RootAST>(std::move(vars), std::move(funcs));

            if (isconst(_token.get_type())) {
                _unget();
                auto res = _analyse_var_decl();
                _rdp_move_back(vars, res);
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
                _rdp_move_back(vars, res);
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

    // <var-decl> ::= [<const>]<type><init-decl-list>';'
    ast::_ptrs<ast::VarDeclAST> RDP::_analyse_var_decl() {
        using namespace ast;

        auto vars = _ptrs<VarDeclAST>();
        bool f_const = false;

        (void) _get();
        // [<const>]
        if (isconst(_token.get_type())) {
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

    // <func-def> ::= <type><identifier><params><block>
    ast::_ptr<ast::FuncDefAST> RDP::_analyse_func_def() {
        using namespace ast;

        (void) _get();
        // <type>
        Type type = _analyse_type_specifier();
        if (type == Type::UNDEFINED) return nullptr;

        // <identifier>
        auto id = _analyser_id();
        if (!id) return nullptr;

        // <params>
        auto params = _analyse_params();

        // <block>
        auto block = _analyse_block();
        if (!block) return nullptr;

        return std::make_unique<FuncDefAST>(type, std::move(id), std::move(params), std::move(block));
    }

    // <params> ::= '(' [<params-decl-list>] ')'
    ast::_ptrs<ast::ParamAST> RDP::_analyse_params() {
        using namespace ast;

        auto params = _ptrs<ParamAST>();

        // '('
        if (!_get() || _token.get_type() != TokenType::LPARENTHESIS) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissParenthesis));
            return { };
        }

        // <params-decl-list> ::= <param-decl>{','<param-decl>}
        // <param-decl>       ::= [<const>]<type><identifier>
        while (true) {
            bool f_const = false;

            if (!_get()) {
                _errs.emplace_back(_rdp_err(ErrCode::ErrMissParenthesis));
                return _rdp_ptrs(params);
            }

            // [<const>]
            if (isconst(_token.get_type())) {
                f_const = true;

                if (!_get()) {
                    _errs.emplace_back(_rdp_err(ErrCode::ErrMissTypeSpecifier));
                    return _rdp_ptrs(params);
                }
            }

            // <type>
            Type type = _analyse_type_specifier();
            if (type == Type::UNDEFINED) return _rdp_ptrs(params);

            // <identifier>
            auto id = _analyser_id();
            if (!id) return _rdp_ptrs(params);

            if (!_get()) {
                _errs.emplace_back(_rdp_err(ErrCode::ErrMissParenthesis));
                return _rdp_ptrs(params);
            }
            // {','<param-decl>}
            if (_token.get_type() == TokenType::RPARENTHESIS)
                break;
            else if (_token.get_type() == TokenType::COMMA)
                params.push_back(std::make_unique<ParamAST>(type, std::move(id), f_const));
            else {
                _errs.emplace_back(_rdp_err(ErrCode::ErrMissParenthesis));
                return _rdp_ptrs(params);
            }
        }

        // already read ')'
        return _rdp_ptrs(params);
    }

    // <block> ::= '{' {<var-decl>}{<stmts>} '}'
    ast::_ptr<ast::BlockStmtAST> RDP::_analyse_block() {
        using namespace ast;

        auto vars = _ptrs<VarDeclAST>();
        auto stmts = _ptrs<StmtAST>();

        // '{'
        if (!_get() || _token.get_type() != TokenType::LBRACE) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissBrace));
            return nullptr;
        }

        // {<var-decl>}
        while (true) {
            if (!_get()) {
                _errs.emplace_back(_rdp_err(ErrCode::ErrMissBrace));
                return std::make_unique<BlockStmtAST>(std::move(vars), std::move(stmts));
            }

            if (isconst(_token.get_type()) || make_type(_token.get_type()) != Type::UNDEFINED) {
                _unget();
                auto res = _analyse_var_decl();
                _rdp_move_back(vars, res);
            } else {
                _unget();
                break;
            }
        }

        // {<stmts>}
        while (true) {
            if (!_get()) {
                _errs.emplace_back(_rdp_err(ErrCode::ErrMissBrace));
                return std::make_unique<BlockStmtAST>(std::move(vars), std::move(stmts));
            }

            // '}'
            if (_token.get_type() == TokenType::RBRACE)
                break;

            _unget();
            if (auto res = _analyse_stmt(); res)
                stmts.push_back(std::move(res));
        }

        return std::make_unique<BlockStmtAST>(std::move(vars), std::move(stmts));
    }

    // <type> !! must pre-read
    inline ast::Type RDP::_analyse_type_specifier() {
        using namespace ast;

        Type type = make_type(_token.get_type());
        if (type == Type::UNDEFINED) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrInvalidTypeSpecifier));
            return Type::UNDEFINED;
        }

        return type;
    }

    // <statement> ::=
    //     <block>|<cond-stmt>|<loop-stmt>|<jmp-stmt>
    //     |<print-stmt>|<scan-stmt>|<assign>';'|<func-call>';'|';'
    ast::_ptr<ast::StmtAST> RDP::_analyse_stmt() {
        using namespace ast;

        if (!_get()) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissSemi));
            return nullptr;
        }

        switch (_token.get_type()) {
            case TokenType::LBRACE: {
                _unget();
                auto res = _analyse_block();
                return _rdp_stmt(res);
            } case TokenType ::IF: {
                _unget();
                auto res = _analyse_if_else();
                return _rdp_stmt(res);
            } case TokenType::WHILE: {
                _unget();
                auto res = _analyse_while();
                return _rdp_stmt(res);
            } case TokenType::RETURN: {
                _unget();
                auto res = _analyse_return();
                return _rdp_stmt(res);
            } case TokenType::PRINT: {
                _unget();
                auto res = _analyse_print();
                return _rdp_stmt(res);
            } case TokenType::SCAN: {
                _unget();
                auto res = _analyse_scan();
                return _rdp_stmt(res);
            } case TokenType::IDENTIFIER: {
                if (!_get()) {
                    _errs.emplace_back(_rdp_err(ErrCode::ErrMissSemi));
                    return nullptr;
                }

                if (auto type = _token.get_type(); type == TokenType::LPARENTHESIS) {
                    _unget(); _unget();
                    auto res = _analyse_func_call();
                    return _rdp_stmt(res);
                } else if (type == TokenType::EQUAL) {
                    _unget(); _unget();
                    auto res = _analyse_assign();
                    return _rdp_stmt(res);
                } else {
                    _unget();
                    _errs.emplace_back(_rdp_err(ErrCode::ErrInvalidStatement));
                    return nullptr;
                }
            }
            default:
                _errs.emplace_back(_rdp_err(ErrCode::ErrInvalidStatement));
                return nullptr;
        }
    }

    // <if-else-stmt> ::= 'if' '(' <cond> ')' <stmt> ['else' <stmt>]
    ast::_ptr<ast::IfElseStmtAST> RDP::_analyse_if_else() {
        using namespace ast;

        // 'if'
        (void) _get();

        // '('
        if (!_get() || _token.get_type() != TokenType::LPARENTHESIS) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissParenthesis));
            return nullptr;
        }

        // <cond>
        auto cond = _analyse_cond();
        if (!cond) return nullptr;

        // ')'
        if (!_get() || _token.get_type() != TokenType::RPARENTHESIS) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissParenthesis));
            return nullptr;
        }

        // <stmt>
        auto s_true = _analyse_stmt();
        if (!s_true) return nullptr;

        if (!_get())
            return std::make_unique<IfElseStmtAST>(std::move(cond), std::move(s_true), nullptr);

        // ['else' <stmt>]
        if (_token.get_type() == TokenType::ELSE) {
            auto s_false = _analyse_stmt();
            if (!s_false)
                return std::make_unique<IfElseStmtAST>(std::move(cond), std::move(s_true), nullptr);
            return std::make_unique<IfElseStmtAST>(std::move(cond), std::move(s_true), std::move(s_false));
        }

        _unget();
        return std::make_unique<IfElseStmtAST>(std::move(cond), std::move(s_true), nullptr);
    }

    // <while-stmt> ::= 'while' '(' <cond> ')' <stmt>
    ast::_ptr<ast::WhileStmtAST> RDP::_analyse_while() {
        using namespace ast;

        // 'while'
        (void) _get();

        // '('
        if (!_get() || _token.get_type() != TokenType::LPARENTHESIS) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissParenthesis));
            return nullptr;
        }

        // <cond>
        auto cond = _analyse_cond();
        if (!cond) return nullptr;

        // ')'
        if (!_get() || _token.get_type() != TokenType::RPARENTHESIS) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissParenthesis));
            return nullptr;
        }

        // <stmt>
        auto stmt = _analyse_stmt();
        if (!stmt) return nullptr;

        return std::make_unique<WhileStmtAST>(std::move(cond), std::move(stmt));
    }

    // <return-stmt> ::= 'return' [<expr>] ';'
    ast::_ptr<ast::ReturnStmtAST> RDP::_analyse_return() {
        using namespace ast;

        // 'return'
        (void) _get();

        if (!_get()) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissSemi));
            return nullptr;
        }

        // ';'
        if (_token.get_type() == TokenType::SEMI)
            return std::make_unique<ReturnStmtAST>(nullptr);

        // [<expr>]
        auto expr = _analyse_expr();
        if (!expr) return nullptr;

        // ';'
        if (!_get()) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissSemi));
            return nullptr;
        }
        return std::make_unique<ReturnStmtAST>(std::move(expr));
    }

    // <print-stmt> ::= 'print' '(' [<print-list>] ')' ';'
    ast::_ptr<ast::PrintStmtAST> RDP::_analyse_print() {
        using namespace ast;

        // 'print'
        (void) _get();

        // '('
        if (!_get() || _token.get_type() != TokenType::LPARENTHESIS) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissParenthesis));
            return nullptr;
        }

        // TODO
    }

    // <scan-stmt> ::= 'scan' '(' <identifier> ')' ';'
    ast::_ptr<ast::ScanStmtAST> RDP::_analyse_scan() {
        using namespace ast;

        // 'scan'
        (void) _get();

        // '('
        if (!_get() || _token.get_type() != TokenType::LPARENTHESIS) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissParenthesis));
            return nullptr;
        }

        auto id = _analyser_id();
        if (!id) return nullptr;

        // ')'
        if (!_get() || _token.get_type() != TokenType::RPARENTHESIS) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissParenthesis));
            return nullptr;
        }

        // ';'
        if (!_get()) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissSemi));
            return nullptr;
        }

        return std::make_unique<ScanStmtAST>(std::move(id));
    }

    ast::_ptr<ast::ExprAST> RDP::_analyse_expr() {
        using namespace ast;

        return nullptr;
    }

    ast::_ptr<ast::CondExprAST> RDP::_analyse_cond() {
        using namespace ast;

        return nullptr;
    }

    ast::_ptr<ast::AssignAST> RDP::_analyse_assign() {
        using namespace ast;

        return nullptr;
    }

    ast::_ptr<ast::FuncCallAST> RDP::_analyse_func_call() {
        using namespace ast;

        return nullptr;
    }

    // <identifier>
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
