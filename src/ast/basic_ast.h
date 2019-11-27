/*
 * Created by zx5 on 2019/11/26.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_BASIC_AST_H
#define C0_BASIC_AST_H

namespace cc0 {
    class AST {
    public:
        virtual void generate() = 0;
    };
}

#endif //C0_BASIC_AST_H
