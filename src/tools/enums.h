/*
 * Created by zx5 on 2019/11/28.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_ENUMS_H
#define C0_ENUMS_H

namespace cc0 {
    namespace ast {
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
            NEQ,
            UNDEFINED,
        };
    }

    enum class ConstantType: unsigned char {
        INT,
        DOUBLE,
        STRING,
    };

    enum class InstructionType: unsigned char {
        NOP,
    };
}

#endif //C0_ENUMS_H
