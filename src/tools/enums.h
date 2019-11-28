/*
 * Created by zx5 on 2019/11/28.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_ENUMS_H
#define C0_ENUMS_H

namespace cc0::ast {
    // TODO: 计算时转换的优先级
    enum class Type: unsigned char {
        VOID,
        INT,
        CHAR,
        STRING,
        DOUBLE,
        UNDEFINED,
    };

    enum class Op: unsigned char {
        ADD,
        SUB,
        MUL,
        DIV,
        LT,
        LE,
        GT,
        GE,
        EQ,
        NEQ
    };
}

#endif //C0_ENUMS_H
