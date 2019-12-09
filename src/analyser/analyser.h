/*
 * Created by zx5 on 2019/11/22.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_ANALYSER_H
#define C0_ANALYSER_H

#include "ctx/rt_ctx.h"

#include <string>
#include <vector>
#include <memory>

namespace cc0 {
    class Analyser {
    protected:
        std::vector<Token>& _tokens;
        int64_t _ptr;
        Token _token;

        std::vector<C0Err> _errs;
        std::vector<C0Err> _wrns;

        [[nodiscard]] inline bool _get() {
            if (_ptr >= static_cast<int64_t>(_tokens.size()))
                return false;

            _token = _tokens.at(_ptr++);
            return true;
        }

        inline void _unget() {
            --_ptr;
            _token = _ptr <= 0 ? _tokens.at(0) : _tokens.at(_ptr - 1);
        }

        Analyser(): _tokens(RuntimeContext::get_tokens()), _ptr(0) {
            if (!_tokens.empty())
                _token = _tokens.at(_ptr);
            else {
                RuntimeContext::put_fatal(C0Err(ErrCode::ErrMissMainEntry,
                                                { 0, 0 }, { 0, 0 }));
                RuntimeContext::set_ast(nullptr);
            }
        }

    public:
        virtual void analyse() = 0;
    };
}

#endif //C0_ANALYSER_H
