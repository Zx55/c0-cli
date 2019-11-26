/*
 * Created by zx5 on 2019/11/22.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_ERROR_H
#define C0_ERROR_H

#include "fmt/format.h"

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

        friend std::ostream& operator<<(std::ostream& out, const C0Err& err) {
            out << fmt::format("{}", err);
            return out;
        }

        friend std::ostream& operator<<(std::ostream& out, const std::vector<C0Err>& errs) {
            if (errs.empty()) {
                out << "No errors :)" << std::endl;
                return out;
            }

            out << fmt::format("|{:>5s}|{:>11s}|\n", "code", "pos");
            out << std::string(19, '-') << std::endl;

            for (const auto &err: errs)
                out << err << std::endl;
            return out;
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

namespace fmt {
    template<>
    struct formatter<cc0::C0Err> {
        constexpr auto parse(format_parse_context& ctx) {
            return ctx.begin();
        }

        template <typename FormatContext>
        auto format(const cc0::C0Err& err, FormatContext &ctx) {
            return format_to(ctx.out(), "|{:>5d}| ({:>3d},{:3d}) |", static_cast<int>(err.get_code()),
                    err.get_pos().first, err.get_pos().second);
        }
    };
}

#endif //C0_ERROR_H
