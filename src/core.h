/*
 * Created by zx5 on 2019/11/22.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_CORE_H
#define C0_CORE_H

#include "ctx/rt_ctx.h"
#include "ctx/src_ctx.h"
#include "ctx/gen_ctx.h"

#include "lexer/lexer.h"
#include "analyser/rdp.h"
#include "ist/generator.h"

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

        ast::_ptr<Analyser> analyser = std::make_unique<RDP>();
        analyser->analyse();
        analyser.reset(nullptr);

        if (output && RuntimeContext::get_fatal().empty()) {
            auto out = std::ofstream(SourceContext::get_out());
            RuntimeContext::get_ast()->graphize(out, 0);
            out.close();
        }

        if (verbose)
            _verbose();
    }

    void compile(bool output = true, bool verbose = true) {
        analyse(false, false);
        if (!RuntimeContext::get_fatal().empty()) {
            if (verbose)
                _verbose();
            return;
        }

        auto gen = Generator();
        gen.generate();

        if (output && RuntimeContext::get_fatal().empty()) {
            auto out = std::ofstream(SourceContext::get_out());
            cc0::Generator::output_ist(out);
            out.close();
        }

        if (verbose)
            _verbose();
    }

    void assemble() {
        compile(false, false);
        if (!RuntimeContext::get_fatal().empty()) {
            _verbose();
            return;
        }

        auto out = std::ofstream(SourceContext::get_out(), std::ios::out | std::ios::binary | std::ios::trunc);
        cc0::Generator::output_bin(out);
        out.close();

        _verbose();
    }
}

#endif //C0_CORE_H
