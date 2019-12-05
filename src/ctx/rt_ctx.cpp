/*
 * Created by zx5 on 2019/11/26.
 * Copyright (c) zx5. All rights reserved.
 */

/* GCC-8.3.0 on compilers-env don't support STL parallel algorithm. */

#include "rt_ctx.h"

namespace cc0 {
    std::vector<C0Err> RuntimeContext::_fatal;
    std::vector<C0Err> RuntimeContext::_wrns;

    std::vector<Token> RuntimeContext::_tokens;
    std::unique_ptr<AST> RuntimeContext::_ast = nullptr;

    std::vector<Instruction> RuntimeContext::_ist;

    inline void RuntimeContext::clear_ctx() {
        _fatal.clear();
        _wrns.clear();

        _tokens.clear();
        _ast.reset(nullptr);
    }

    std::vector<C0Err> RuntimeContext::get_errs() {
        [[maybe_unused]] auto cmp = [](const C0Err& lhs, const C0Err& rhs) {
            if (lhs.get_start().first == rhs.get_start().first)
                return lhs.get_start().second <= rhs.get_start().second;
            return lhs.get_start().first <= rhs.get_start().first;
        };

        std::vector<C0Err> errs(_fatal);
        if (SourceContext::_f_wall)
            errs.insert(errs.end(), _wrns.begin(), _wrns.end());

        std::sort(errs.begin(), errs.end(), cmp);
        return errs;
    }
}