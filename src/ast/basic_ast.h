/*
 * Created by zx5 on 2019/11/26.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_BASIC_AST_H
#define C0_BASIC_AST_H

#include "tools/error.h"

#include <vector>

namespace cc0::ast {
    class AST {
    public:
        // TODO: the range of AST, for error in semantic parse.
        // TODO: what to return?
        // virtual std::vector<C0Err> generate() = 0;
        // virtual void graphize(std::ostream& out, int t) = 0;
    };
}

#endif //C0_BASIC_AST_H
