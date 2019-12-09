/*
 * Created by zx5 on 2019/11/27.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_ERROR_H
#define C0_ERROR_H

#include <utility>

#include "tools/alias.h"
#include "ctx/src_ctx.h"

#include "fmt/format.h"

#define ERR_NUM 34
#define _wrn_offset(_code) ((_code) - 128 + ERR_NUM)

namespace cc0 {
    enum class ErrCode: unsigned char {
        ErrEOF,
        ErrInvalidInput,
        ErrInvalidFloat,
        ErrInvalidHexadecimal,
        ErrInvalidChar,
        ErrInvalidString,
        ErrInvalidEscape,
        ErrInvalidDeclaration,
        ErrInvalidTypeSpecifier,
        ErrInvalidExpression,
        ErrInvalidStatement,
        ErrInvalidCase,
        ErrInvalidForUpdate,
        ErrMissEqOp,
        ErrMissAssignOp,
        ErrMissQuote,
        ErrMissSemi,
        ErrMissTypeSpecifier,
        ErrMissIdentifier,
        ErrMissParenthesis,
        ErrMissBrace,
        ErrMissColon,
        ErrMissWhile,
        ErrRepeatedDefault,
        ErrIncompleteComment,
        ErrUninitailizedConstant,
        ErrUndeclaredIdentifier,
        ErrRedeclaredIdentifier,
        ErrAssignToConstant,
        ErrVoidHasNoValue,
        ErrIncomparable,
        ErrJmpInAcyclicStmt,
        ErrParameterUnMatch,
        ErrMissMainEntry,
        WrnInt32Overflow            = 128,
        WrnFloat64Overflow,
        WrnEmptyStatement,
        WrnUnreachableCase,
        WrnUninitailizedVariable,
    };

    class C0Err final {
        friend inline void swap(C0Err& lhs, C0Err& rhs) {
            std::swap(lhs._code, rhs._code);
            std::swap(lhs._range, rhs._range);
        }

        friend std::ostream& operator<<(std::ostream& out, const C0Err& err) {
            out << fmt::format("{}", err);
            return out;
        }

        friend std::ostream& operator<<(std::ostream& out, const std::vector<C0Err>& errs) {
            if (errs.empty())
                return out;

            for (const auto& err: errs)
                out << err << '\n';
            return out;
        }

    private:
        ErrCode _code;
        range_t _range;

    public:
        explicit C0Err(ErrCode code, range_t range):
            _code(code), _range(std::move(range)) { }
        explicit C0Err(ErrCode code, pos_t start, pos_t end):
            C0Err(code, { start, end }) { }
        explicit C0Err(ErrCode code, int64_t start_row, int64_t start_col, int64_t end_row, int64_t end_col):
            C0Err(code, { start_row, start_col }, { end_row, end_col }) { }
        C0Err(const C0Err&) = default;
        C0Err(C0Err&& err) noexcept: _code(err._code), _range(std::move(err._range)) {}
        C0Err& operator=(C0Err rhs) {
            swap(*this, rhs);
            return *this;
        }
        bool operator==(const C0Err& err) { return _code == err._code && _range == err._range; }

        [[nodiscard]] inline bool is_fatal() const { return static_cast<int64_t>(_code) <= 127; }
        [[nodiscard]] inline ErrCode get_code() const { return _code; }
        [[nodiscard]] inline std::string get_err_line() const { return SourceContext::get_line(get_start().first); }
        [[nodiscard]] inline pos_t get_start() const { return _range.first; }
        [[nodiscard]] inline pos_t get_end() const { return _range.second; }
        [[nodiscard]] inline int64_t get_len() const { return get_end().second - get_start().second; }
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
                    "ErrInvalidDeclaration",
                    "ErrInvalidTypeSpecifier",
                    "ErrInvalidExpression",
                    "ErrInvalidStatement",
                    "ErrInvalidCase",
                    "ErrInvalidForUpdate",
                    "ErrMissEqOp",
                    "ErrMissAssignOp",
                    "ErrMissQuote",
                    "ErrMissSemi",
                    "ErrMissTypeSpecifier",
                    "ErrMissIdentifier",
                    "ErrMissParenthesis",
                    "ErrMissBrace",
                    "ErrMissColon",
                    "ErrMissWhile",
                    "ErrRepeatedDefault",
                    "ErrIncompleteComment",
                    "ErrUninitailizedConstant",
                    "ErrUndeclaredVariable",
                    "ErrRedeclaredIdentifier",
                    "ErrAssignToConstant",
                    "ErrVoidHasNoValue",
                    "ErrIncomparable",
                    "ErrJmpInAcyclicStmt",
                    "ErrParameterUnMatch",
                    "ErrMissMainEntry",
                    "WrnInt32Overflow",
                    "WrnFloat64Overflow",
                    "WrnEmptyStatement",
                    "WrnUnreachableCase",
                    "WrnUninitailizedVariable",
            };

            auto row = err.get_start().first + 1, col = err.get_start().second + 1;
            auto code = static_cast<int64_t>(err.get_code());
            code = err.is_fatal() ? code : _wrn_offset(code);
            auto arrow = std::string(err.get_start().second + 8, ' ') + std::string(err.get_len(), '^');

            return format_to(ctx.out(), "{:s} In file {}:{:d}:{:d}: {:s}\n {:4d} | {:s}{:s}",
                    err.is_fatal() ? "!!Error!!" : "!Warning!", cc0::SourceContext::get_in_absolute(),
                    row, col, err_printable[code], row, err.get_err_line(), arrow);
        }
    };
}

#endif //C0_ERROR_H
