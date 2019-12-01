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
#define _rdp_bin(_lhs, _op, _rhs) std::unique_ptr<ExprAST>( \
    new BinaryExprAST(std::move(_lhs), _op, std::move(_rhs)))
#define _rdp_expr(_res) std::unique_ptr<ExprAST>(std::move(_res))

namespace cc0 {
    // <C0> ::= {<var-decl>}{<func-def>}
    ast::_ptr<ast::RootAST> RDP::_analyse_root() {
        using namespace ast;
        using namespace utils;

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
                    _unget();
                    _unget();
                    _unget();
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
        using namespace utils;

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
        // <init-decl>      ::= <identifier>['='<expr>]
        while (true) {
            // <identifier>
            auto id = _analyse_id();
            if (!id) return _rdp_ptrs(vars);

            if (!_get()) {
                _errs.emplace_back(_rdp_err(ErrCode::ErrMissSemi));
                return _rdp_ptrs(vars);
            }
            // ['='<expr>]
            if (_token.get_type() == TokenType::ASSIGN) {
                if (auto res = _analyse_expr(); res)
                    vars.push_back(std::make_unique<VarDeclAST>(type, std::move(id), std::move(res), f_const));
            } else {
                _unget();
                vars.push_back(std::make_unique<VarDeclAST>(type, std::move(id), nullptr, f_const));
            }

            if (!_get()) {
                _errs.emplace_back(_rdp_err(ErrCode::ErrMissSemi));
                return _rdp_ptrs(vars);
            }
            // {','<init-decl>}
            if (_token.get_type() == TokenType::SEMI)
                break;
            else if (_token.get_type() == TokenType::COMMA)
                continue;
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
        using namespace utils;

        (void) _get();
        // <type>
        Type type = _analyse_type_specifier();
        if (type == Type::UNDEFINED) return nullptr;

        // <identifier>
        auto id = _analyse_id();
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
        using namespace utils;

        auto params = _ptrs<ParamAST>();

        // '('
        if (!_get() || _token.get_type() != TokenType::LPARENTHESIS) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissParenthesis));
            return { };
        }

        if (!_get()) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissParenthesis));
            return { };
        }
        // ')'
        if (_token.get_type() == TokenType::RPARENTHESIS)
            return { };
        else
            _unget();

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
            auto id = _analyse_id();
            if (!id) return _rdp_ptrs(params);
            params.push_back(std::make_unique<ParamAST>(type, std::move(id), f_const));

            if (!_get()) {
                _errs.emplace_back(_rdp_err(ErrCode::ErrMissParenthesis));
                return _rdp_ptrs(params);
            }

            // {','<param-decl>}
            if (auto token_t = _token.get_type(); token_t == TokenType::RPARENTHESIS)
                break;
            else if (token_t == TokenType::COMMA)
                continue;
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
        using namespace utils;

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
        using namespace utils;

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
        using namespace utils;

        if (!_get()) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissSemi));
            return nullptr;
        }

        switch (_token.get_type()) {
            case TokenType::LBRACE: {
                _unget();
                auto res = _analyse_block();
                return _rdp_stmt(res);
            }
            case TokenType ::IF: {
                _unget();
                auto res = _analyse_if_else();
                return _rdp_stmt(res);
            }
            case TokenType::WHILE: {
                _unget();
                auto res = _analyse_while();
                return _rdp_stmt(res);
            }
            case TokenType::DO: {
                _unget();
                auto res = _analyse_do_while();
                return _rdp_stmt(res);
            }
            case TokenType::FOR: {
                _unget();
                auto res = _analyse_for();
                return _rdp_stmt(res);
            }
            case TokenType::BREAK: {
                return std::make_unique<BreakStmtAST>();
            }
            case TokenType::CONTINUE: {
                return std::make_unique<ContinueStmtAST>();
            }
            case TokenType::RETURN: {
                _unget();
                auto res = _analyse_return();
                return _rdp_stmt(res);
            }
            case TokenType::PRINT: {
                _unget();
                auto res = _analyse_print();
                return _rdp_stmt(res);
            }
            case TokenType::SCAN: {
                _unget();
                auto res = _analyse_scan();
                return _rdp_stmt(res);
            }
            case TokenType::IDENTIFIER: {
                if (!_get()) {
                    _errs.emplace_back(_rdp_err(ErrCode::ErrMissSemi));
                    return nullptr;
                }

                if (auto type = _token.get_type(); type == TokenType::LPARENTHESIS) {
                    _unget(); _unget();
                    auto res = _analyse_func_call();
                    return _rdp_stmt(res);
                } else if (type == TokenType::ASSIGN) {
                    _unget(); _unget();
                    auto res = _analyse_assign();
                    if (!_get() || _token.get_type() != TokenType::SEMI)
                        _errs.emplace_back(_rdp_err(ErrCode::ErrMissSemi));
                    return _rdp_stmt(res);
                } else {
                    _unget();
                    _errs.emplace_back(_rdp_err(ErrCode::ErrInvalidStatement));
                    return nullptr;
                }
            }
            case TokenType::SEMI: {
                _wrns.emplace_back(_rdp_err(ErrCode::WrnEmptyStatement));
                return std::unique_ptr<StmtAST>(new EmptyStmtAST());
            }
            default:
                _errs.emplace_back(_rdp_err(ErrCode::ErrInvalidStatement));
                return nullptr;
        }
    }

    // <if-else-stmt> ::= 'if' '(' <cond> ')' <stmt> ['else' <stmt>]
    ast::_ptr<ast::IfElseStmtAST> RDP::_analyse_if_else() {
        using namespace ast;
        using namespace utils;

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
        using namespace utils;

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

    // <do-while-stmt> ::= 'do' <stmt> 'while' '(' <cond> ')' ';'
    ast::_ptr<ast::DoWhileStmtAST> RDP::_analyse_do_while() {
        using namespace ast;
        using namespace utils;

        // 'do'
        (void) _get();
        // <stmt>
        auto stmt = _analyse_stmt();
        if (!stmt) return nullptr;

        // 'while'
        if (!_get() || _token.get_type() != TokenType::WHILE) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissWhile));
            return nullptr;
        }

        // '('
        if (!_get() ||_token.get_type() != TokenType::LPARENTHESIS) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissParenthesis));
            return nullptr;
        }

        // <cond>
        auto cond = _analyse_cond();
        if (!cond) return nullptr;

        // ')'
        if (!_get() || _token.get_type() != TokenType::RPARENTHESIS)
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissParenthesis));
        if (!_get() || _token.get_type() != TokenType::SEMI)
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissSemi));
        return std::make_unique<DoWhileStmtAST>(std::move(cond), std::move(stmt));
    }

    // TODO: <for-stmt> ::= 'for' '('<for-init> [<cond>]';' [<for-update>]')' <stmt>
    ast::_ptr<ast::ForStmtAST> RDP::_analyse_for() {
        using namespace ast;
        using namespace utils;

        (void) _get();

        return nullptr;
    }

    // <return-stmt> ::= 'return' [<expr>] ';'
    ast::_ptr<ast::ReturnStmtAST> RDP::_analyse_return() {
        using namespace ast;
        using namespace utils;

        // 'return'
        (void) _get();

        if (!_get()) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissSemi));
            return std::make_unique<ReturnStmtAST>(nullptr);
        }

        // ';'
        if (_token.get_type() == TokenType::SEMI)
            return std::make_unique<ReturnStmtAST>(nullptr);

        _unget();
        // [<expr>]
        auto expr = _analyse_expr();
        if (!expr) return nullptr;

        // ';'
        if (!_get() || _token.get_type() != TokenType::SEMI)
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissSemi));
        return std::make_unique<ReturnStmtAST>(std::move(expr));
    }

    // <print-stmt> ::= 'print' '(' [<print-list>] ')' ';'
    ast::_ptr<ast::PrintStmtAST> RDP::_analyse_print() {
        using namespace ast;
        using namespace utils;

        auto printable = _ptrs<ExprAST>();

        // 'print'
        (void) _get();

        // '('
        if (!_get() || _token.get_type() != TokenType::LPARENTHESIS) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissParenthesis));
            return nullptr;
        }

        if (!_get()) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissParenthesis));
            return nullptr;
        }
        // ')'
        if (_token.get_type() == TokenType::RPARENTHESIS)
            return std::make_unique<PrintStmtAST>(std::move(printable));
        else
            _unget();

        // [<print-list>]
        while (true) {
            if (!_get()) {
                _errs.emplace_back(_rdp_err(ErrCode::ErrMissParenthesis));
                return std::make_unique<PrintStmtAST>(std::move(printable));
            }

            // <printable> ::= <expr> | <string>
            if (_token.get_type() == TokenType::STRING_LITERAL)
                printable.push_back(std::make_unique<StringExprAST>(_token.get_value_str()));
            else {
                _unget();
                if (auto res = _analyse_expr(); res)
                    printable.push_back(std::move(res));
            }

            // {',' <printable>}
            if (!_get()) {
                _errs.emplace_back(_rdp_err(ErrCode::ErrMissParenthesis));
                return std::make_unique<PrintStmtAST>(std::move(printable));
            }
            if (auto type = _token.get_type(); type == TokenType::RPARENTHESIS)
                break;
            else if (type == TokenType::COMMA)
                continue;
            else {
                _errs.emplace_back(_rdp_err(ErrCode::ErrMissParenthesis));
                return std::make_unique<PrintStmtAST>(std::move(printable));
            }
        }

        // already read ')'
        // ';'
        if (!_get() || _token.get_type() != TokenType::SEMI)
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissSemi));
        return std::make_unique<PrintStmtAST>(std::move(printable));
    }

    // <scan-stmt> ::= 'scan' '(' <identifier> ')' ';'
    ast::_ptr<ast::ScanStmtAST> RDP::_analyse_scan() {
        using namespace ast;
        using namespace utils;

        // 'scan'
        (void) _get();

        // '('
        if (!_get() || _token.get_type() != TokenType::LPARENTHESIS) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissParenthesis));
            return nullptr;
        }

        auto id = _analyse_id();
        if (!id) return nullptr;

        // ')'
        if (!_get() || _token.get_type() != TokenType::RPARENTHESIS) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissParenthesis));
            return nullptr;
        }

        // ';'
        if (!_get() || _token.get_type() != TokenType::SEMI)
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissSemi));

        return std::make_unique<ScanStmtAST>(std::move(id));
    }

    // <expr> ::= <term>{<add-op><term>}
    ast::_ptr<ast::ExprAST> RDP::_analyse_expr() {
        using namespace ast;
        using namespace utils;

        auto root = _analyse_term();
        if (!root) return nullptr;

        while (true) {
            if (!_get())
                return root;

            if (auto type = _token.get_type(); isadd(type)) {
                auto op = make_op(type);
                if (auto rhs = _analyse_term(); rhs)
                    root = _rdp_bin(root, op, rhs);
                else
                    return root;
            } else {
                _unget();
                return root;
            }
        }
    }

    // <term> ::= <factor>{<mul-op><factor>}
    ast::_ptr<ast::ExprAST> RDP::_analyse_term() {
        using namespace ast;
        using namespace utils;

        auto root = _analyse_factor();
        if (!root) return nullptr;

        while (true) {
            if (!_get())
                return root;

            if (auto type = _token.get_type(); ismul(type)) {
                auto op = make_op(type);
                if (auto rhs = _analyse_factor(); rhs)
                    root = _rdp_bin(root, op, rhs);
                else
                    return root;
            } else {
                _unget();
                return root;
            }
        }
    }

    // <factor> ::= {'(' <type> ')'} [<unary-op>] <prim-expr>
    ast::_ptr<ast::ExprAST> RDP::_analyse_factor() {
        using namespace ast;
        using namespace utils;

        std::vector<Type> casts;
        bool sign = true;

        if (!_get()) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrInvalidExpression));
            return nullptr;
        }

        // {'(' <type> ')'}
        while (_token.get_type() == TokenType::LPARENTHESIS) {
            if (!_get()) {
                _errs.emplace_back(_rdp_err(ErrCode::ErrMissTypeSpecifier));
                return nullptr;
            }

            auto type = _analyse_type_specifier();
            if (type == Type::UNDEFINED) return nullptr;
            casts.push_back(type);

            if (!_get() || _token.get_type() != TokenType::RPARENTHESIS) {
                _errs.emplace_back(_rdp_err(ErrCode::ErrMissParenthesis));
                return nullptr;
            }

            if (!_get()) {
                _errs.emplace_back(_rdp_err(ErrCode::ErrInvalidExpression));
                return nullptr;
            }
        }

        if (auto type = _token.get_type(); isadd(type)) {
            sign = (type == TokenType::PLUS);

            if (!_get()) {
                _errs.emplace_back(_rdp_err(ErrCode::ErrInvalidExpression));
                return nullptr;
            }
        }

        _unget();
        auto root = _analyse_primary();
        if (!root) return nullptr;

        if (!sign)
            root = std::unique_ptr<ExprAST>(new UnaryExprAST(sign, std::move(root)));
        for (auto it = casts.crbegin(); it != casts.crend(); ++it)
            root = std::unique_ptr<ExprAST>(new CastExprAST(*it, std::move(root)));

        return root;
    }

    // <prim-expr> ::= '('<expr>')'|<identifier>|<int-literal>
    //    |<char-literal>|<floating-literal>|<func-call>
    ast::_ptr<ast::ExprAST> RDP::_analyse_primary() {
        using namespace ast;
        using namespace utils;

        (void) _get();
        switch (_token.get_type()) {
            case TokenType::LPARENTHESIS: {
                auto expr = _analyse_expr();
                if (!expr) return nullptr;

                if (!_get() || _token.get_type() != TokenType::RPARENTHESIS)
                    _errs.emplace_back(_rdp_err(ErrCode::ErrMissParenthesis));

                return _rdp_expr(expr);
            }
            case TokenType::IDENTIFIER: {
                if (!_get())
                    goto L_ID;
                if (_token.get_type() == TokenType::LPARENTHESIS) {
                    // <func-call>
                    _unget(); _unget();
                    if (auto call = _analyse_func_call(); call)
                        return _rdp_expr(call);
                } else
                    _unget();

            L_ID:
                // <identifier>
                _unget();
                auto id = _analyse_id();
                return _rdp_expr(id);
            }
            case TokenType::DECIMAL:
                [[fallthrough]];
            case TokenType::HEXADECIMAL:
                return std::unique_ptr<ExprAST>(new Int32ExprAST(
                        std::any_cast<int32_t>(_token.get_value())));
            case TokenType::FLOAT:
                return std::unique_ptr<ExprAST>(new Float64ExprAST(
                        std::any_cast<double>(_token.get_value())));
            case TokenType::CHAR_LITERAL: {
                char ch = _token.get_value_str().at(0);
                return std::unique_ptr<ExprAST>(new CharExprAST(ch));
            }
            default:
                _errs.emplace_back(_rdp_err(ErrCode::ErrInvalidExpression));
                return nullptr;
        }
    }

    // <cond-expr> ::= <expr>[<re-op><expr>]
    ast::_ptr<ast::CondExprAST> RDP::_analyse_cond() {
        using namespace ast;
        using namespace utils;

        auto lhs = _analyse_expr();
        if (!lhs) return nullptr;

        if (!_get())
            return std::make_unique<CondExprAST>(std::move(lhs));

        if (auto type = _token.get_type(); isre_op(type)) {
            auto op = make_op(type);
            if (auto rhs = _analyse_expr(); rhs)
                return std::make_unique<CondExprAST>(std::move(lhs), op, std::move(rhs));
            else
                return nullptr;
        }

        _unget();
        return std::make_unique<CondExprAST>(std::move(lhs));
    }

    // <assign-expr> ::= <identifier>'='<expr>
    ast::_ptr<ast::AssignAST> RDP::_analyse_assign() {
        using namespace ast;
        using namespace utils;

        auto id = _analyse_id();
        if (!id) return nullptr;

        if (!_get() || _token.get_type() != TokenType::ASSIGN) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissAssignOp));
            return nullptr;
        }

        auto value = _analyse_expr();
        if (!value) return nullptr;

        return std::make_unique<AssignAST>(std::move(id), std::move(value));
    }

    // <func-call> ::= <identifier> '(' [<expr-list>] ')'
    ast::_ptr<ast::FuncCallAST> RDP::_analyse_func_call() {
        using namespace ast;
        using namespace utils;

        auto params = _ptrs<ExprAST>();

        // <identifier>
        auto id = _analyse_id();
        if (!id) return nullptr;

        // '('
        if (!_get() || _token.get_type() != TokenType::LPARENTHESIS) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissParenthesis));
            return nullptr;
        }

        if (!_get()) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissParenthesis));
            return nullptr;
        }
        // ')'
        if (_token.get_type() == TokenType::RPARENTHESIS)
            return std::make_unique<FuncCallAST>(std::move(id), std::move(params));
        else
            _unget();

        // [<expr-list>]
        while (true) {
            if (!_get()) {
                _errs.emplace_back(_rdp_err(ErrCode::ErrMissParenthesis));
                return std::make_unique<FuncCallAST>(std::move(id), std::move(params));
            }

            _unget();
            if (auto param = _analyse_expr(); param)
                params.push_back(std::move(param));

            // {',' <expr>}
            if (!_get()) {
                _errs.emplace_back(_rdp_err(ErrCode::ErrMissParenthesis));
                return std::make_unique<FuncCallAST>(std::move(id), std::move(params));
            }

            if (auto type = _token.get_type(); type == TokenType::RPARENTHESIS)
                break;
            else if (type == TokenType::COMMA)
                continue;
            else {
                _errs.emplace_back(_rdp_err(ErrCode::ErrMissParenthesis));
                return std::make_unique<FuncCallAST>(std::move(id), std::move(params));
            }
        }

        // already read ')'
        return std::make_unique<FuncCallAST>(std::move(id), std::move(params));
    }

    // <identifier>
    inline ast::_ptr<ast::IdExprAST> RDP::_analyse_id() {
        using namespace ast;
        using namespace utils;

        if (!_get() || _token.get_type() != TokenType::IDENTIFIER) {
            _errs.emplace_back(_rdp_err(ErrCode::ErrMissIdentifier));
            return nullptr;
        }

        return std::make_unique<IdExprAST>(_token);
    }

    void RDP::analyse() {
        using namespace ast;
        using namespace utils;

        RuntimeContext::set_ast(_analyse_root());
        RuntimeContext::put_fatals(std::move(_errs));
        RuntimeContext::put_wrns(std::move(_wrns));
    }
}
