/*
 * Created by zx5 on 2019/11/22.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_FATAL_H
#define C0_FATAL_H

#include "basic_error.h"
#include "ctx/src_ctx.h"

#include "fmt/format.h"

#include <string>
#include <utility>

namespace cc0 {
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

    class C0Err final: public BasicError<ErrCode> {
        // FIXME: print C0Err when using stringstream as input
        friend std::ostream& operator<<(std::ostream& out, const C0Err& err) {
            out << fmt::format("{}", err);
            return out;
        }

        friend std::ostream& operator<<(std::ostream& out, const std::vector<C0Err>& errs) {
            if (errs.empty()) {
                out << "No errors :)" << std::endl;
                return out;
            }

            for (const auto& err: errs)
                out << err << std::endl;
            return out;
        }

    public:
        explicit C0Err(ErrCode code, range_t range): BasicError(code, range) { }
        explicit C0Err(ErrCode code, pos_t start, pos_t end): BasicError(code, start, end) { }
        explicit C0Err(ErrCode code, int64_t start_row, int64_t start_col, int64_t end_row, int64_t end_col):
            BasicError(code, start_row, start_col, end_row, end_col) { }
        C0Err(const C0Err& err) = default;
        C0Err(C0Err&& err) noexcept: BasicError(std::move(err)) { }
        C0Err& operator=(C0Err err) {
            swap(*this, err);
            return *this;
        }
        bool operator==(const C0Err& err) { return BasicError::operator==(err); }
    };
}

namespace fmt {
    template<>
    struct formatter<cc0::C0Err> {
        constexpr auto parse(format_parse_context& ctx) {
            return ctx.begin();
        }

        template <typename FormatContext>
        auto format(const cc0::C0Err& err, FormatContext& ctx) {
            static std::vector<std::string> err_printable = {
                    "ErrEOF",
                    "ErrInvalidInput",
                    "ErrInvalidFloat",
                    "ErrInvalidHexadecimal",
                    "ErrInvalidChar",
                    "ErrInvalidString",
                    "ErrInvalidEscape",
                    "ErrInt32Overflow",
                    "ErrFloatOverflow",
                    "ErrMissEqOp",
                    "ErrMissQuote",
                    "ErrIncompleteComment",
            };

            // TODO: change console color using ASNI escape
            auto row = err.get_start().first + 1;
            return format_to(ctx.out(), "In file {}:{:d}:{:d}: error: {:s}\n {:4d} | {:s}{:s}\n",
                    cc0::SourceContext::get_absolute(), row, err.get_start().second + 1,
                    err_printable[static_cast<int>(err.get_code())], row, err.get_err_line(),
                    std::string(err.get_start().second + 8, ' ') + std::string(err.get_len(), '^'));
        }
    };
}

#endif //C0_FATAL_H
