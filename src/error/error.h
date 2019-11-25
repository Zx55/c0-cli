/*
 * Created by zx5 on 2019/11/22.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_ERROR_H
#define C0_ERROR_H

#include <utility>

namespace cc0 {
    using int32_t = std::int32_t;
    using uint32_t = std::uint32_t;
    using int64_t = std::int64_t;
    using uint64_t = std::uint64_t;

    using pos_t = std::pair<uint64_t, uint64_t>;

    enum class ErrCode: char {
        ErrEOF,
        ErrInvalidInput,
        ErrInvalidFloat,
        ErrInvalidHexadecimal,
        ErrInvalidChar,
        ErrInvalidString,
        ErrInvalidEscape,
        ErrInt32Overflow,
        ErrFloatOverflow,
        ErrMissEqOp,
        ErrMissQuote,
        ErrIncompleteComment,
    };

    class C0Err final {
        friend inline void swap(C0Err& lhs, C0Err& rhs) {
            std::swap(lhs._code, rhs._code);
            std::swap(lhs._pos, rhs._pos);
        }
    public:
        C0Err(ErrCode code, pos_t pos): _code(code), _pos(std::move(pos)) { }
        C0Err(ErrCode code, uint64_t row, uint64_t col): _code(code), _pos({ row, col }) { }
        C0Err(const C0Err &err) = default;
        C0Err(C0Err &&err) noexcept: _code(err._code), _pos(std::move(err._pos)) { }
        C0Err& operator=(C0Err err) { swap(*this, err); return *this; }
        bool operator==(const C0Err &err) { return _code == err._code && _pos == err._pos; }

        [[nodiscard]] ErrCode get_code() const { return _code; }
        [[nodiscard]] pos_t get_pos() const { return _pos; }

    private:
        ErrCode _code;
        pos_t _pos;
    };
}

#endif //C0_ERROR_H
