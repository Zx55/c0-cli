/*
 * Created by zx5 on 2019/11/27.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_BASIC_ERROR_H
#define C0_BASIC_ERROR_H

#include "tools/alias.h"
#include "ctx/src_ctx.h"

#include <string>

namespace cc0 {
    template <typename _Code>
    class BasicError {
        friend inline void swap(BasicError& lhs, BasicError& rhs) {
            std::swap(lhs._code, rhs._code);
            swap(lhs._ctx, rhs._ctx);
        }
    private:
        class _ErrContext final {
            friend inline void swap(_ErrContext& lhs, _ErrContext& rhs) {
                std::swap(lhs._line, rhs._line);
                std::swap(lhs._range, rhs._range);
            }

        public:
            std::string& _line;
            range_t _range;

            _ErrContext(std::string& line, range_t range):
                    _line(line), _range(std::move(range)) { }
            bool operator==(const _ErrContext& ctx) const { return _range == ctx._range; }

            [[nodiscard]] inline std::string get_line() const { return _line; }
            [[nodiscard]] inline pos_t get_start() const { return _range.first; }
            [[nodiscard]] inline pos_t get_end() const { return _range.second; }
            [[nodiscard]] inline int64_t get_len() const { return get_end().second - get_start().second; }
        };

        _Code _code;
        _ErrContext _ctx;

    public:
        explicit BasicError(_Code code, range_t range):
            _code(code), _ctx(SourceContext::get_line(range.first.first), range) { }
        explicit BasicError(_Code code, pos_t start, pos_t end):
            BasicError(code, { start, end }) { }
        explicit BasicError(_Code code, int64_t start_row, int64_t start_col, int64_t end_row, int64_t end_col):
            BasicError(code, { start_row, start_col }, { end_row, end_col }) { }
        BasicError(const BasicError&) = default;
        BasicError(BasicError&& err) noexcept:
            _code(err._code), _ctx(std::move(err._ctx)) {}
        BasicError& operator=(BasicError err) {
            swap(*this, err);
            return *this;
        }
        bool operator==(const BasicError& err) { return _code == err._code && _ctx == err._ctx; }

        [[nodiscard]] inline _Code get_code() const { return _code; }
        [[nodiscard]] inline std::string get_err_line() const { return _ctx.get_line(); }
        [[nodiscard]] inline pos_t get_start() const { return _ctx.get_start(); }
        [[nodiscard]] inline pos_t get_end() const { return _ctx.get_end(); }
        [[nodiscard]] inline int64_t get_len() const { return _ctx.get_len(); }
    };
}

#endif //C0_BASIC_ERROR_H
