/*
 * Created by zx5 on 2019/12/4.
 * Copyright (c) zx5. All rights reserved.
 */

#include "gen_ctx.h"

namespace cc0 {
    SymTbl GeneratorContext::_tbl;
    std::vector<C0Err> GeneratorContext::_fatal;
    std::vector<C0Err> GeneratorContext::_wrns;
    std::vector<Instruction> GeneratorContext::_ist;
}
