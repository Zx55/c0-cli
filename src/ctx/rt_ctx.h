/*
 * Created by zx5 on 2019/11/26.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_RT_CTX_H
#define C0_RT_CTX_H

#include "tools/error.h"
#include "tools/utils.h"
#include "src_ctx.h"

#include "lexer/token.h"
#include "ast/ast.h"
#include "instruction/instruction.h"
#include "symtbl/symtbl.h"

#include <algorithm>
#include <vector>
#include <string>
#include <memory>

namespace cc0 {
    class RuntimeContext final {
        friend class Lexer;
        friend class RDP;
        friend class ast::RootAST;
        friend class ast::VarDeclAST;
        friend class ast::AssignAST;
        friend class ast::FuncDefAST;
        friend class ast::ParamAST;
        friend class ast::BlockStmtAST;
        friend class ast::FuncCallAST;
        friend class ast::Int32ExprAST;
        friend class ast::Float64ExprAST;
        friend class ast::CharExprAST;
        friend class ast::StringExprAST;
        friend class ast::IdExprAST;
        friend class ast::CondExprAST;
        friend class ast::CastExprAST;
        friend class ast::BinaryExprAST;
        friend class ast::UnaryExprAST;
        friend class ast::EmptyStmtAST;
        friend class ast::IfElseStmtAST;
        friend class ast::LabelStmtAST;
        friend class ast::SwitchStmtAST;
        friend class ast::WhileStmtAST;
        friend class ast::ForStmtAST;
        friend class ast::DoWhileStmtAST;
        friend class ast::ReturnStmtAST;
        friend class ast::BreakStmtAST;
        friend class ast::ContinueStmtAST;
        friend class ast::PrintStmtAST;
        friend class ast::ScanStmtAST;

    private:
        static std::vector<C0Err> _fatal;
        static std::vector<C0Err> _wrns;

        static std::vector<Token> _tokens;
        static std::unique_ptr<AST> _ast;

        static SymTbl tbl;
        static std::vector<Instruction> _inst;

    public:
        RuntimeContext() = delete;
        RuntimeContext(const RuntimeContext&) = delete;
        RuntimeContext(RuntimeContext&&) = delete;
        RuntimeContext& operator=(const RuntimeContext&) = delete;
        ~RuntimeContext() {
            // release static this unique_ptr
            if (_ast != nullptr) {
                _ast.reset(nullptr);
            }
        }

        inline static void clear_ctx();

        [[nodiscard]] inline static std::vector<C0Err> get_fatal() {
            return SourceContext::_f_werror ? get_errs() : _fatal;
        }
        [[nodiscard]] inline static auto& get_wrns() { return _wrns; }
        [[nodiscard]] static std::vector<C0Err> get_errs();

        [[nodiscard]] inline static auto& get_tokens() { return _tokens; }
        [[nodiscard]] inline static auto& get_ast() { return _ast; }
        [[nodiscard]] inline static auto& get_tbl() { return tbl; }
        [[nodiscard]] inline static auto& get_instructions() { return _inst; }

    private:
        inline static void put_fatals(std::vector<C0Err>&& errs) {
            _fatal.insert(_fatal.end(), std::make_move_iterator(errs.begin()),
                    std::make_move_iterator(errs.end()));
        }
        inline static void put_fatal(C0Err err) { _fatal.push_back(std::move(err)); }
        inline static void put_wrns(std::vector<C0Err>&& wrns) {
            _wrns.insert(_wrns.end(), std::make_move_iterator(wrns.begin()),
                    std::make_move_iterator(wrns.end()));
        }
        inline static void put_wrn(C0Err wrn) { _wrns.push_back(std::move(wrn)); }

        inline static void set_tokens(std::vector<Token>&& tokens) { _tokens = std::move(tokens); }
        inline static void put_token(Token token) { _tokens.push_back(std::move(token)); }
        inline static void set_ast(ast::_ptr<AST> ast) { _ast.reset(nullptr); _ast = std::move(ast); }
        inline static void put_inst(Instruction inst) { _inst.push_back(std::move(inst)); }
    };
}

#endif //C0_RT_CTX_H
