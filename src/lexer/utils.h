/*
 * Created by zx5 on 2019/11/25.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_LEX_UTILS_H
#define C0_LEX_UTILS_H

#include <cctype>

#define IS_FUNC(fn) \
    inline bool fn(int ch) { \
        return std::fn(static_cast<unsigned char>(ch)); \
    } \

namespace cc0 {
    IS_FUNC(isprint)
    IS_FUNC(isblank)
    IS_FUNC(isdigit)
    IS_FUNC(isalpha)
    IS_FUNC(isalnum)
    IS_FUNC(ispunct)

    inline bool isline(int ch) {
        return ch == '\r' || ch == '\n';
    }

    inline bool isspace(int ch) {
        return isblank(ch) || isline(ch);
    }

    inline bool ishex(int ch) {
        return isdigit(ch) || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
    }

    inline bool iseof(int ch) {
        return ch == '\0';
    }

    // 可接受字符集
    inline bool isaccept(int ch) {
        return isspace(ch) || isalnum(ch) || ispunct(ch);
    }

    inline bool isc_char(int ch) {
        return isaccept(ch) && !isline(ch) && ch != '\'' && ch != '\\';
    }

    inline bool iss_char(int ch) {
        return isaccept(ch) && !isline(ch) && ch != '\"' && ch != '\\';
    }

    // 转义字符 \x特判
    inline bool ise_char(int ch) {
        return ch == 'r' || ch == 'n' || ch == 't' || ch == '\"' || ch == '\'' || ch == '\\';
    }
}

#endif //C0_LEX_UTILS_H
