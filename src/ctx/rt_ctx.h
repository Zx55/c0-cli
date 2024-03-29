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
#include "ist/instruction.h"

#include <algorithm>
#include <vector>
#include <string>
#include <memory>

namespace cc0 {
    class RuntimeContext final {
        friend class Lexer;
        friend class Analyser;
        friend class RDP;
        friend class Generator;

    private:
        static std::vector<C0Err> _fatal;
        static std::vector<C0Err> _wrns;

        static std::vector<Token> _tokens;
        static std::unique_ptr<ast::RootAST> _ast;
        static std::vector<Instruction> _ist;

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

        inline static void clear_ctx() {
            _fatal.clear();
            _wrns.clear();

            _tokens.clear();
            _ast.reset(nullptr);
            _ist.clear();
        }

        [[nodiscard]] inline static std::vector<C0Err> get_fatal() {
            return SourceContext::_f_werror ? get_errs() : _fatal;
        }
        [[nodiscard]] inline static auto& get_wrns() { return _wrns; }
        [[nodiscard]] static std::vector<C0Err> get_errs();

        [[nodiscard]] inline static auto& get_tokens() { return _tokens; }
        [[nodiscard]] inline static auto& get_ast() { return _ast; }
        [[nodiscard]] inline static auto& get_ist() { return _ist; }

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
        inline static void set_ast(ast::_ptr<ast::RootAST> ast) { _ast.reset(nullptr); _ast = std::move(ast); }
        inline static void set_ist(std::vector<Instruction> inst) { _ist = std::move(inst); }
    };
}

#endif //C0_RT_CTX_H
