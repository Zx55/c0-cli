/*
 * Created by zx5 on 2019/11/28.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_ENUMS_H
#define C0_ENUMS_H

namespace cc0 {
    // 类别表
    enum class TokenType: char {
        UNDEFINED,

        CONST,
        VOID,
        INT,
        CHAR,
        DOUBLE,
        STRUCT,
        IF,
        ELSE,
        SWITCH,
        CASE,
        DEFAULT,
        WHILE,
        FOR,
        DO,
        RETURN,
        BREAK,
        CONTINUE,
        PRINT,
        SCAN,

        DECIMAL,
        HEXADECIMAL,
        FLOAT,
        IDENTIFIER,
        PLUS,
        MINUS,
        MULTIPLY,
        DIVISION,
        ASSIGN,

        EQUAL,
        GREATER,
        GREATER_EQUAL,
        LESS,
        LESS_EQUAL,
        NEQUAL,

        SEMI,
        COLON,
        COMMA,

        LPARENTHESIS,
        RPARENTHESIS,
        LBRACE,
        RBRACE,

        CHAR_LITERAL,
        STRING_LITERAL,
    };

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

    using ast::Type;

    enum class InstType: unsigned char {
        NOP,
        BIPUSH, IPUSH,
        POP, POP2, POPN,
        DUP, DUP2,
        LOADC, LOADA,
        SNEW,
        ILOAD, DLOAD,
        ISTORE, DSTORE,
        IADD, DADD, ISUB, DSUB,
        IMUL, DMUL, IDIV, DDIV,
        INEG, DNEG,
        ICMP, DCMP,
        I2D, D2I, I2C,
        JMP, JE, JNE, JGE, JG, JLE, JL,
        CALL, RET, IRET, DRET,
        IPRINT, DPRINT, CPRINT, SPRINT, PRINTL,
        ISCAN, DSCAN, CSCAN
    };
}

#endif //C0_ENUMS_H
