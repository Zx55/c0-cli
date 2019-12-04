/*
 * Created by zx5 on 2019/12/4.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_GENERATOR_H
#define C0_GENERATOR_H

#include "tools/alias.h"
#include "ctx/rt_ctx.h"
#include "symtbl/symtbl.h"

namespace cc0 {
    class Generator {
    private:
        ast::_ptr<AST>& _root;
        static SymTbl _tbl;

        std::vector<Instruction> _ist;

    public:
        Generator(): _root(RuntimeContext::get_ast()) { }
        Generator(const Generator&) = delete;
        Generator(Generator&&) = delete;
        Generator& operator=(const Generator&) = delete;

        [[nodiscard]] inline static SymTbl& get_tbl() { return _tbl; }

        inline void generate() {
            _ist.clear();
            _ist = std::move(_root->generate({
                0, 0, 0, Type::UNDEFINED
            })._ist);
            RuntimeContext::set_ist(_ist);
        }
    };
}

#endif //C0_GENERATOR_H
