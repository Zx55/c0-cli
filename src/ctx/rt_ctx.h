/*
 * Created by zx5 on 2019/11/26.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_RT_CTX_H
#define C0_RT_CTX_H

#include "lexer/token.h"
#include "tools/alias.h"
#include "tools/error.h"

#include <vector>
#include <string>

namespace cc0 {
    class   RuntimeContext final {
        friend class Lexer;
    private:
        static std::vector<Token> _tokens;
        static std::vector<C0Err> _errs;

    public:
        RuntimeContext() = delete;
        RuntimeContext(const RuntimeContext&) = delete;
        RuntimeContext(RuntimeContext&&) = delete;
        RuntimeContext& operator=(const RuntimeContext&) = delete;

        inline static void clear_ctx();
        [[nodiscard]] inline static auto& get_tokens() { return _tokens; }
        [[nodiscard]] inline static auto& get_errs() { return _errs; }


    private:
        inline static void set_tokens(std::vector<Token> tokens) { _tokens = std::move(tokens); }
        inline static void put_token(Token token) { _tokens.push_back(std::move(token)); }
        inline static void set_errs(std::vector<C0Err> errs) { _errs = std::move(errs); }
        inline static void put_err(C0Err err) { _errs.push_back(std::move(err)); }
    };
}

#endif //C0_RT_CTX_H
