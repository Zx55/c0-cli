/*
 * Created by zx5 on 2019/11/22.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_LEXER_H
#define C0_LEXER_H

#include "token.h"

#include <string>
#include <unordered_map>

namespace cc0 {
    class Lexer final {
    private:
        // 读入EOF抛出异常
        class _EOFException final {};

        enum class DFAState: char {

        };

        std::istream &_in;
        Token _token;
        int _ch;

    public:
    };
}

#endif //C0_LEXER_H
