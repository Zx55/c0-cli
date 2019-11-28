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
        [[nodiscard]] ast::_ptr<ast::ExprAST> _analyse_expr();
        [[nodiscard]] inline ast::_ptr<ast::IdExprAST> _analyser_id();

    public:
        RDP(): Analyser() { }
        RDP(const RDP&) = delete;
        RDP(RDP&&) = delete;
        RDP& operator=(const RDP&) = delete;

        void analyse() override;
    };
}

#endif //C0_RDP_H
