/*
 * Created by zx5 on 2019/11/28.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_JMP_STMT_H
#define C0_JMP_STMT_H

#include "basic_stmt.h"

namespace cc0::ast {
    class ReturnStmtAST final: public StmtAST {
    private:
        _ptr<ExprAST> _ret;

        [[nodiscard]] inline static InstType _make_ret(Type type) {
            switch (type) {
                case Type::VOID:
                    return InstType::RET;
                case Type::INT:
                    [[fallthrough]];
                case Type::CHAR:
                    return InstType::IRET;
                case Type::DOUBLE:
                    return InstType::DRET;
                default:
                    return InstType::NOP;
            }
        }

    public:
        explicit ReturnStmtAST(range_t range, _ptr<ExprAST> ret = nullptr):
            StmtAST(range), _ret(std::move(ret)) { }

        void graphize(std::ostream& out, int t) override {
            out << "<ret-stmt>\n";
            if (_ret != nullptr) {
                out << _end(t);
                _ret->graphize(out, t + 1);
            }
        }

        [[nodiscard]] _GenResult generate(_GenParam param) override {
            uint32_t len = 0;
            Type ret_type = (_ret == nullptr) ? Type::VOID : _ret->get_type();
            if (ret_type == Type::UNDEFINED)
                return _gen_ret(0);

            // type check
            if (param._ret != ret_type && (param._ret == Type::VOID || ret_type == Type::VOID)) {
                _gen_err(ErrCode::ErrVoidHasNoValue);
                return _gen_ret(0);
            }

            // push return value on the top
            if (_ret != nullptr) {
                auto res = _ret->generate({ param._level, param._offset, param._slot,
                                            param._ret, false });
                if (res._len == 0)
                    return _gen_ret(0);
                len += res._len;
            }

            if (param._ret == ret_type) {
                _gen_ist0(_make_ret(ret_type));
                return _gen_ret(len + 1);
            }

            // type transform
            switch (ret_type) {
                case Type::DOUBLE:
                    _gen_ist0(InstType::D2I);
                    if (param._ret == Type::CHAR) {
                        _gen_ist0(InstType::I2C);
                        ++len;
                    }
                    ++len;
                    break;
                case Type::INT:
                    if (param._ret == Type::DOUBLE)
                        _gen_ist0(InstType::I2D);
                    else if (param._ret == Type::CHAR)
                        _gen_ist0(InstType::I2C);
                    ++len;
                    break;
                case Type::CHAR:
                    if (param._ret == Type::DOUBLE) {
                        _gen_ist0(InstType::I2D);
                        ++len;
                    }
                    break;
                default:
                    return _gen_ret(0);
            }

            _gen_ist0(_make_ret(param._ret));
            return _gen_ret(len + 1);
        }
    };

    class BreakStmtAST final: public StmtAST {
    public:
        explicit BreakStmtAST(range_t range): StmtAST(range) { }

        void graphize(std::ostream& out, [[maybe_unused]] int t) override {
            out << "<break-stmt>\n";
        }

        [[nodiscard]] _GenResult generate([[maybe_unused]] _GenParam param) override {
            _gen_ist1(InstType::IPUSH, 0);
            _gen_ist0(InstType::POPN);
            _gen_ist1(InstType::JMP, 0);
            return { 3, { {_gen_ist_off - 1,
                           _symtbl.get_slot_by_level(), get_range() } }, {} };
        }
    };

    class ContinueStmtAST final: public StmtAST {
    public:
        explicit ContinueStmtAST(range_t range): StmtAST(range) { }

        void graphize(std::ostream& out, [[maybe_unused]] int t) override {
            out << "<continue-stmt>\n";
        }

        [[nodiscard]] _GenResult generate([[maybe_unused]] _GenParam param) override {
            _gen_ist1(InstType::IPUSH, 0);
            _gen_ist0(InstType::POPN);
            _gen_ist1(InstType::JMP, 0);
            return { 3, {}, { { _gen_ist_off - 1,
                                _symtbl.get_slot_by_level(), get_range() } } };
        }
    };
}

#endif //C0_JMP_STMT_H
