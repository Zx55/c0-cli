/*
 * Created by zx5 on 2019/11/28.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_IO_STMT_H
#define C0_IO_STMT_H

#include "basic_stmt.h"

namespace cc0::ast {
    class PrintStmtAST final: public StmtAST {
    private:
        _ptrs<ExprAST> _printable;

        [[nodiscard]] inline static InstType _make_print(Type type) {
            switch (type) {
                case Type::INT:
                    return InstType::IPRINT;
                case Type::CHAR:
                    return InstType::CPRINT;
                case Type::DOUBLE:
                    return InstType::DPRINT;
                case Type::STRING:
                    return InstType::SPRINT;
                default:
                    return InstType::NOP;
            }
        }

    public:
        explicit PrintStmtAST(range_t range, _ptrs<ExprAST> printable):
            StmtAST(range), _printable(std::move(printable)) { }

        void graphize(std::ostream& out, int t) override {
            out << "<print-stmt>\n";
            _graphize_list(_printable, out, t, t + 1);
        }

        [[nodiscard]] _GenResult generate(_GenParam param) override {
            uint32_t len = 0;

            for (const auto& printable: _printable) {
                auto type = printable->get_type();
                if (type == Type::UNDEFINED)
                    continue;
                if (type == Type::VOID) {
                    _gen_err(ErrCode::ErrVoidHasNoValue);
                    continue;
                }

                auto res = printable->generate({ param._level, param._offset, param._slot,
                                                 param._ret, false });
                if (res._len == 0) continue;
                _gen_ist0(_make_print(type));
                _gen_ist1(InstType::BIPUSH, 32);
                _gen_ist0(InstType::CPRINT);
                len += res._len + 3;
            }

            _gen_popn(2);
            _gen_ist0(InstType::PRINTL);
            return _gen_ret(len - 1);
        }
    };

    class ScanStmtAST final: public StmtAST {
    private:
        _ptr<IdExprAST> _id;

        [[nodiscard]] inline static InstType _make_scan(Type type) {
            switch (type) {
                case Type::INT:
                    return InstType::ISCAN;
                case Type::CHAR:
                    return InstType::CSCAN;
                case Type::DOUBLE:
                    return InstType::DSCAN;
                default:
                    return InstType::NOP;
            }
        }

        [[nodiscard]] inline static InstType _make_store(Type type) {
            if (type == Type::DOUBLE)
                return InstType::DSTORE;
            return InstType::ISTORE;
        }

    public:
        explicit ScanStmtAST(range_t range, _ptr<IdExprAST> id):
            StmtAST(range), _id(std::move(id)) { }

        void graphize(std::ostream& out, int t) override {
            out << "<scan-stmt> ";
            _id->graphize(out, t + 1);
        }

        [[nodiscard]] _GenResult generate(_GenParam param) override {
            if (_id->get_type() == Type::UNDEFINED)
                return _gen_ret(0);

            auto var = _symtbl.get_var(_id->get_id_str());
            if (var->is_const()) {
                _gen_err(ErrCode::ErrAssignToConstant);
                return _gen_ret(0);
            }

            // load address
            auto res = _id->generate({ param._level, param._offset, param._slot, Type::UNDEFINED });
            if (res._len == 0)
                return _gen_ret(0);

            _gen_ist0(_make_scan(_id->get_type()));
            _gen_ist0(_make_store(_id->get_type()));
            _symtbl.var_init(_id->get_id_str());
            return _gen_ret(res._len + 2);
        }
    };
}

#endif //C0_IO_STMT_H
