/*
 * Created by zx5 on 2019/12/4.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_GENERATOR_H
#define C0_GENERATOR_H

#include "tools/alias.h"
#include "ctx/rt_ctx.h"
#include "ctx/gen_ctx.h"

namespace cc0 {
    class Generator {
    private:
        ast::_ptr<AST>& _root;

    public:
        Generator(): _root(RuntimeContext::get_ast()) { }
        Generator(const Generator&) = delete;
        Generator(Generator&&) = delete;
        Generator& operator=(const Generator&) = delete;

        inline void generate() {
            GeneratorContext::init_tbl();
            (void) _root->generate({ 0, 0, 0, Type::UNDEFINED });

            RuntimeContext::set_ist(std::move(GeneratorContext::get_ist()));
            RuntimeContext::put_fatals(std::move(GeneratorContext::get_fatals()));
            RuntimeContext::put_wrns(std::move(GeneratorContext::get_wrns()));
        }
    };
}

#endif //C0_GENERATOR_H
