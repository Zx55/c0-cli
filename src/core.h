/*
 * Created by zx5 on 2019/11/22.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_CORE_H
#define C0_CORE_H

#include "lexer/lexer.h"

#include "ctx/rt_ctx.h"
#include "ctx/src_ctx.h"

#include <fstream>

namespace cc0 {
    void tokenize() {
        auto lexer = Lexer();
        lexer.all_tokens();

        auto out = std::ofstream(SourceContext::get_out());
        out << "Tokens: \n";
        out << cc0::RuntimeContext::get_tokens();
        out.close();

        if (auto errs = cc0::RuntimeContext::get_errs(); !errs.empty())
            std::cerr << errs << std::endl;
    }
}

#endif //C0_CORE_H
