/*
 * Created by zx5 on 2019/11/22.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_ANALYSER_H
#define C0_ANALYSER_H

#include "tools/utils.h"

#include "tools/alias.h"
#include "tools/error.h"
#include "ctx/rt_ctx.h"

#include "lexer/token.h"
#include "ast/ast.h"

#include <string>
#include <vector>
#include <memory>

namespace cc0 {
    class Analyser {
    protected:
        std::vector<Token>& _tokens;
        int64_t _ptr;
        Token& _token;

        std::vector<C0Err> _errs;
        std::vector<C0Err> _wrns;

        [[nodiscard]] inline bool _get() {
            if (_ptr >= static_cast<int64_t>(_tokens.size()))
                return false;

            _token = _tokens.at(_ptr++);
            return true;
        }

        inline void _unget() { --_ptr; _token = _tokens.at(_ptr); }

        Analyser(): _tokens(RuntimeContext::get_tokens()), _ptr(0), _token(_tokens.at(_ptr)) { }

    public:
        virtual void analyse() = 0;
    };
}

#endif //C0_ANALYSER_H
