/*
 * Created by zx5 on 2019/11/27.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_RDP_H
#define C0_RDP_H

#include "analyser.h"

namespace cc0 {
    class RDP final: public Analyser {
    private:
        [[nodiscard]] ast::_ptr<ast::RootAST> _analyse_root();
        [[nodiscard]] ast::_ptrs<ast::VarDeclAST> _analyse_var_decl();
        [[nodiscard]] ast::_ptr<ast::FuncDefAST> _analyse_func_def();
        [[nodiscard]] ast::_ptrs<ast::ParamAST> _analyse_params();
        [[nodiscard]] ast::_ptr<ast::BlockStmtAST> _analyse_block();
        [[nodiscard]] inline ast::Type _analyse_type_specifier();
        [[nodiscard]] ast::_ptr<ast::StmtAST> _analyse_stmt();
        [[nodiscard]] ast::_ptr<ast::IfElseStmtAST> _analyse_if_else();
        [[nodiscard]] ast::_ptr<ast::SwitchStmtAST> _analyse_switch();
        [[nodiscard]] ast::_ptr<ast::WhileStmtAST> _analyse_while();
        [[nodiscard]] ast::_ptr<ast::DoWhileStmtAST> _analyse_do_while();
        [[nodiscard]] ast::_ptr<ast::ForStmtAST> _analyse_for();
        [[nodiscard]] ast::_ptr<ast::ReturnStmtAST> _analyse_return();
        [[nodiscard]] ast::_ptr<ast::PrintStmtAST> _analyse_print();
        [[nodiscard]] ast::_ptr<ast::ScanStmtAST> _analyse_scan();
        [[nodiscard]] ast::_ptr<ast::ExprAST> _analyse_expr();
        [[nodiscard]] ast::_ptr<ast::ExprAST> _analyse_term();
        [[nodiscard]] ast::_ptr<ast::ExprAST> _analyse_factor();
        [[nodiscard]] ast::_ptr<ast::ExprAST> _analyse_primary();
        [[nodiscard]] ast::_ptr<ast::CondExprAST> _analyse_cond();
        [[nodiscard]] ast::_ptr<ast::AssignAST> _analyse_assign();
        [[nodiscard]] ast::_ptr<ast::FuncCallAST> _analyse_func_call();
        [[nodiscard]] inline ast::_ptr<ast::IdExprAST> _analyse_id();

    public:
        RDP(): Analyser() { }
        RDP(const RDP&) = delete;
        RDP(RDP&&) = delete;
        RDP& operator=(const RDP&) = delete;

        void analyse() override;
    };
}

#endif //C0_RDP_H
