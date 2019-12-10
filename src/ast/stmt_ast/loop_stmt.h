/*
 * Created by zx5 on 2019/11/28.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_LOOP_STMT_H
#define C0_LOOP_STMT_H

#include "basic_stmt.h"

namespace cc0::ast {
    class WhileStmtAST final: public StmtAST {
    private:
        _ptr<CondExprAST> _cond;
        _ptr<StmtAST> _stmt;

    public:
        explicit WhileStmtAST(range_t range, _ptr<CondExprAST> cond, _ptr<StmtAST> stmt):
            StmtAST(range), _cond(std::move(cond)), _stmt(std::move(stmt)) { }

        void graphize(std::ostream& out, int t) override {
            out << "<while-stmt>\n" << _mid(t);
            _cond->graphize(out, t + 1);
            out << _end(t);
            _stmt->graphize(out, t + 1);
        }

        [[nodiscard]] _GenResult generate(_GenParam param) override {
            /*
             * `while (cond) stmt` will generate:
             *
             *     jmp .Test
             *     ...stmt
             * .Test
             *     ...cond
             *     j$(op) .Loop
             * .End
             */
            auto jmp_to_test = _gen_ist_off;
            _gen_ist1(InstType::JMP, 0);
            uint32_t len = 1;
            auto loop_label = param._offset + len;

            auto stmt = _stmt->generate({ param._level, param._offset + len,
                                          param._slot, param._ret, true });
            if (stmt._len == 0) {
                _gen_pop;
                return _gen_ret(0);
            }
            len += stmt._len;
            _gen_ist(jmp_to_test).set_op1(param._offset + len);

            auto cond = _cond->generate({ param._level, param._offset, param._slot,
                                          param._ret, false });
            if (cond._len == 0) {
                _gen_popn(len);
                return _gen_ret(0);
            }
            _gen_ist1(_make_jmp(_cond->get_op()), loop_label);
            len += cond._len + 1;

            // handle breaks and continues
            auto slot = _symtbl.get_slot_by_level();
            for (const auto jmp_to_end: stmt._breaks) {
                _gen_ist(jmp_to_end.ist_off - 2).set_op1(jmp_to_end.slot - slot);
                _gen_ist(jmp_to_end.ist_off).set_op1(param._offset + len);
            }

            for (const auto jmp_to_head: stmt._continues) {
                _gen_ist(jmp_to_head.ist_off - 2).set_op1(jmp_to_head.slot - slot);
                _gen_ist(jmp_to_head.ist_off).set_op1(param._offset);
            }

            return _gen_ret(len);
        }
    };

    class AssignAST final: public ExprAST, public StmtAST {
    private:
        _ptr<IdExprAST> _id;
        _ptr<ExprAST> _value;

        [[nodiscard]] inline static InstType _make_store(Type type) {
            if (type == Type::DOUBLE)
                return InstType::DSTORE;
            return InstType::ISTORE;
        }

    public:
        explicit AssignAST(range_t range, _ptr<IdExprAST> id, _ptr<ExprAST> value):
            ExprAST(range, Type::VOID), StmtAST(range), _id(std::move(id)), _value(std::move(value)) { }

        [[nodiscard]] inline Type get_type() override {
            return Type::VOID;
        }

        void graphize(std::ostream& out, int t) override {
            out << "<assignment>\n" << _mid(t);
            _id->graphize(out, t + 1);
            out << _end(t);
            _value->graphize(out, t + 1);
        }

        [[nodiscard]] _GenResult generate(_GenParam param) override {
            auto ltype = _id->get_type(), rtype = _value->get_type();
            if (ltype == Type::UNDEFINED || rtype == Type::UNDEFINED)
                return _gen_ret(0);
            if (rtype == Type::VOID) {
                _gen_err2(ErrCode::ErrVoidHasNoValue);
                return _gen_ret(0);
            }

            auto var = _symtbl.get_var(_id->get_id_str());
            if (var->is_const()) {
                _gen_err2(ErrCode::ErrAssignToConstant);
                return _gen_ret(0);
            }

            uint32_t len = 0;
            auto lhs = _id->generate({ param._level, param._offset, param._slot,
                                       Type::UNDEFINED, false });
            if (lhs._len == 0)
                return _gen_ret(0);
            len += lhs._len;

            auto rhs = _value->generate({ param._level, param._offset, param._slot,
                                          param._ret, false });
            if (rhs._len == 0) {
                _gen_popn(len);
                return _gen_ret(0);
            }
            len += rhs._len;

            if (ltype == rtype) {
                _gen_ist0(_make_store(ltype));
                _symtbl.var_init(_id->get_id_str());
                return _gen_ret(len + 1);
            }

            switch (rtype) {
                case Type::DOUBLE:
                    _gen_ist0(InstType::D2I);
                    if (ltype == Type::CHAR) {
                        _gen_ist0(InstType::I2C);
                        ++len;
                    }
                    ++len;
                    break;
                case Type::INT:
                    if (ltype == Type::DOUBLE)
                        _gen_ist0(InstType::I2D);
                    else if (ltype == Type::CHAR)
                        _gen_ist0(InstType::I2C);
                    ++len;
                    break;
                case Type::CHAR:
                    if (ltype == Type::DOUBLE) {
                        _gen_ist0(InstType::I2D);
                        ++len;
                    }
                    break;
                default:
                    _gen_popn(len);
                    return _gen_ret(0);
            }

            _gen_ist0(_make_store(ltype));
            _symtbl.var_init(_id->get_id_str());
            return _gen_ret(len);
        }
    };

    class ForStmtAST final: public StmtAST {
    private:
        _ptrs<AssignAST> _init;
        _ptr<CondExprAST> _cond;
        _ptrs<ExprAST> _update;
        _ptr<StmtAST> _stmt;

    public:
        explicit ForStmtAST(range_t range, _ptrs<AssignAST> init, _ptr<CondExprAST> cond,
                _ptrs<ExprAST> update, _ptr<StmtAST> stmt):
            StmtAST(range), _init(std::move(init)), _cond(std::move(cond)),
            _update(std::move(update)), _stmt(std::move(stmt)) { }

        void graphize(std::ostream& out, int t) override {
            out << "<for-stmt>\n";

            if (!_init.empty())
                out << ((_cond == nullptr && _update.empty() && _stmt == nullptr) ? _end(t) : _mid(t))
                    << "<init-stmt>\n";
            _graphize_list(_init, out, t + 1, t + 2);

            if (_cond != nullptr) {
                out << ((_update.empty() && _stmt == nullptr) ? _end(t) : _mid(t)) << "<for-cond>";
                _cond->graphize(out, t + 1);
            }

            if (!_update.empty())
                out << (_stmt == nullptr ? _end(t) : _mid(t)) << "<for-update>\n";
            _graphize_list(_update, out, t + 1, t + 2);

            if (_stmt != nullptr) {
                out << _end(t);
                _stmt->graphize(out, t + 1);
            }
        }

        [[nodiscard]] _GenResult generate(_GenParam param) override {
            /* for (init; cond; update) stmt
             *
             *     ...init
             *     jmp .Test
             * .Loop
             *     ...stmt
             * .Update
             *     ...update
             * .Test
             *     ...cond
             *     j$(op) .Loop
             * .End
             *
             * !!we must make continue stmt jmp to .Update rather than .Head
             */
            uint32_t len = 0;

            for (const auto& init: _init) {
                if (auto res = init->generate(param); res._len == 0)
                    continue;
                else
                    len += res._len;
            }

            auto jmp_to_test = _gen_ist_off;
            _gen_ist1(InstType::JMP, 0);
            ++len;

            auto loop_label = param._offset + len;
            auto stmt = _stmt->generate({ param._level, param._offset + len,
                                          param._slot, param._ret, true });
            if (stmt._len == 0) {
                _gen_popn(len);
                return _gen_ret(0);
            }
            len += stmt._len;

            auto update_label = param._offset + len;
            for (const auto& update: _update) {
                if (auto res = update->generate({ param._level, param._offset, param._slot,
                                                  param._ret, true }); res._len == 0)
                    continue;
                else
                    len += res._len;
            }
            _gen_ist(jmp_to_test).set_op1(param._offset + len);

            if (_cond == nullptr) {
                // always true
                _gen_ist1(InstType::JMP, loop_label);
                ++len;
            } else {
                auto cond = _cond->generate({param._level, param._offset, param._slot,
                                             param._ret, false});
                if (cond._len == 0) {
                    _gen_popn(len);
                    return _gen_ret(0);
                }
                _gen_ist1(_make_jmp(_cond->get_op()), loop_label);
                len += cond._len + 1;
            }

            auto slot = _symtbl.get_slot_by_level();
            for (const auto jmp_to_end: stmt._breaks) {
                _gen_ist(jmp_to_end.ist_off - 2).set_op1(jmp_to_end.slot - slot);
                _gen_ist(jmp_to_end.ist_off).set_op1(param._offset + len);
            }
            for (const auto jmp_to_update: stmt._continues) {
                _gen_ist(jmp_to_update.ist_off - 2).set_op1(jmp_to_update.slot - slot);
                _gen_ist(jmp_to_update.ist_off).set_op1(update_label);
            }

            return _gen_ret(len);
        }
    };

    class DoWhileStmtAST final: public StmtAST {
    private:
        _ptr<CondExprAST> _cond;
        _ptr<StmtAST> _stmt;

    public:
        explicit DoWhileStmtAST(range_t range, _ptr<CondExprAST> cond, _ptr<StmtAST> stmt):
            StmtAST(range), _cond(std::move(cond)), _stmt(std::move(stmt)) { }

        void graphize(std::ostream& out, int t) override {
            out << "<do-while-stmt>\n" << _mid(t);
            _cond->graphize(out, t + 1);
            out << _end(t);
            _stmt->graphize(out, t + 1);
        }

        [[nodiscard]] _GenResult generate(_GenParam param) override {
            /*
             * .Loop
             *     ...stmt
             * .Test
             *     ...cond
             *     j$(op) Loop
             * .End
             */
            auto stmt = _stmt->generate(param);
            if (stmt._len == 0)
                return _gen_ret(0);
            auto len = stmt._len;
            auto test_label = param._offset + len;

            auto cond = _cond->generate({ param._level, param._offset, param._slot,
                                          param._ret, false });
            if (cond._len == 0) {
                _gen_popn(len);
                return _gen_ret(0);
            }

            _gen_ist1(_make_jmp(_cond->get_op()), param._offset);
            len += cond._len + 1;

            auto slot = _symtbl.get_slot_by_level();
            for (const auto jmp_to_end: stmt._breaks) {
                _gen_ist(jmp_to_end.ist_off - 2).set_op1(jmp_to_end.slot - slot);
                _gen_ist(jmp_to_end.ist_off).set_op1(param._offset + len);
            }
            for (const auto jmp_to_head: stmt._continues) {
                _gen_ist(jmp_to_head.ist_off - 2).set_op1(jmp_to_head.slot - slot);
                _gen_ist(jmp_to_head.ist_off).set_op1(test_label);
            }

            return _gen_ret(len);
        }
    };
}

#endif //C0_LOOP_STMT_H
