/*
 * Created by zx5 on 2019/11/26.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_RT_CTX_H
#define C0_RT_CTX_H

#include "tools/alias.h"
#include "tools/error.h"
#include "src_ctx.h"

#include "lexer/token.h"
#include "ast/ast.h"

#include <vector>
#include <string>
#include <memory>

namespace cc0 {
    class RuntimeContext final {
        friend class Lexer;
        friend class RDP;

    private:
        static std::vector<C0Err> _fatal;
        static std::vector<C0Err> _wrns;

        static std::vector<Token> _tokens;
        static std::unique_ptr<AST> _ast;

    public:
        RuntimeContext() = delete;
        RuntimeContext(const RuntimeContext&) = delete;
        RuntimeContext(RuntimeContext&&) = delete;
        RuntimeContext& operator=(const RuntimeContext&) = delete;

        inline static void clear_ctx();

        [[nodiscard]] inline static auto& get_fatal() { return _fatal; }
        [[nodiscard]] inline static auto& get_wrns() { return _wrns; }
        [[nodiscard]] static std::vector<C0Err> get_errs();

        [[nodiscard]] inline static auto& get_tokens() { return _tokens; }
        [[nodiscard]] inline static auto& get_ast() { return _ast; }

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
    };
}

#endif //C0_RT_CTX_H
