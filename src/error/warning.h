/*
 * Created by zx5 on 2019/11/27.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_WARNING_H
#define C0_WARNING_H

#include "basic_error.h"
#include "ctx/src_ctx.h"

#include "fmt/format.h"

#include <vector>
#include <string>

namespace cc0 {
    enum class WrnCode: char {
        WrnInt32Overflow,
        WrnFloat64Overflow,
    };

    class C0Wrn final: public BasicError<WrnCode> {
        // FIXME: print C0Wrn when using stringstream as input
        friend std::ostream& operator<<(std::ostream& out, const C0Wrn& wrn) {
            out << fmt::format("{}", wrn);
            return out;
        }

        friend std::ostream& operator<<(std::ostream& out, const std::vector<C0Wrn>& wrns) {
            if (wrns.empty()) {
                out << "No errors :)" << std::endl;
                return out;
            }

            for (const auto& wrn: wrns)
                out << wrn << std::endl;
            return out;
        }

    public:
        explicit C0Wrn(WrnCode code, range_t range): BasicError(code, range) { }
        explicit C0Wrn(WrnCode code, pos_t start, pos_t end): BasicError(code, start, end) { }
        explicit C0Wrn(WrnCode code, int64_t start_row, int64_t start_col, int64_t end_row, int64_t end_col):
            BasicError(code, start_row, start_col, end_row, end_col) { }
        C0Wrn(const C0Wrn& wrn) = default;
        C0Wrn(C0Wrn&& wrn) noexcept: BasicError(std::move(wrn)) { }
        C0Wrn& operator=(C0Wrn wrn) {
            swap(*this, wrn);
            return *this;
        }
        bool operator==(const C0Wrn& wrn) { return BasicError::operator==(wrn); }
    };
}

namespace fmt {
    template<>
    struct formatter<cc0::C0Wrn> {
        constexpr auto parse(format_parse_context& ctx) {
            return ctx.begin();
        }

        template <typename FormatContext>
        auto format(const cc0::C0Wrn& wrn, FormatContext& ctx) {
            static std::vector<std::string> wrn_printable = {
                    "WrnInt32Overflow",
                    "WrnFloat64Overflow",
            };

            // TODO: change console color using ASNI escape
            auto row = wrn.get_start().first + 1;
            return format_to(ctx.out(), "In file {}:{:d}:{:d}: warning: {:s}\n {:4d} | {:s}{:s}\n",
                    cc0::SourceContext::get_absolute(), row, wrn.get_start().second + 1,
                    wrn_printable[static_cast<int>(wrn.get_code())], row, wrn.get_err_line(),
                    std::string(wrn.get_start().second + 8, ' ') + std::string(wrn.get_len(), '^'));
        }
    };
}

#endif //C0_WARNING_H
