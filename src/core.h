/*
 * Created by zx5 on 2019/11/22.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_CORE_H
#define C0_CORE_H

#include "lexer/lexer.h"
#include "analyser/rdp.h"

#include "ctx/rt_ctx.h"
#include "ctx/src_ctx.h"

#include <fstream>

static inline void _verbose() {
    if (auto errs = cc0::RuntimeContext::get_errs(); !errs.empty())
        std::cerr << errs << std::endl;
}

namespace cc0 {
    void tokenize(bool output = true, bool verbose = true) {
        auto lexer = Lexer();
        lexer.all_tokens();

        if (output) {
            auto out = std::ofstream(SourceContext::get_out());
            out << "Tokens: \n";
            out << cc0::RuntimeContext::get_tokens();
            out.close();
        }

        if (verbose)
            _verbose();
    }

    void analyse(bool output = true, bool verbose = true) {
        tokenize(false, false);
        if (!RuntimeContext::get_fatal().empty()) {
            if (verbose)
                _verbose();

            return;
        }

        std::unique_ptr<Analyser> analyser = std::make_unique<RDP>();
        analyser->analyse();
        analyser.reset(nullptr);

        auto& root = RuntimeContext::get_ast();
        if (output) {
            auto out = std::ofstream(SourceContext::get_out());
            root->graphize(out, 0);
            out.close();
        }

        if (verbose)
            _verbose();
    }
}

#endif //C0_CORE_H
