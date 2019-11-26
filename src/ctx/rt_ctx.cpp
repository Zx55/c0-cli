/*
 * Created by zx5 on 2019/11/26.
 * Copyright (c) zx5. All rights reserved.
 */

#include "rt_ctx.h"

namespace cc0 {
    std::vector<Token> RuntimeContext::_tokens;
    std::vector<C0Err> RuntimeContext::_errs;

    inline void RuntimeContext::clear_ctx() {
        _tokens.clear();
        _errs.clear();
    }
}