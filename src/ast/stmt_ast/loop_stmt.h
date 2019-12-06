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
             *     j$(op) .Test
             * .End
             */
            _gen_ist1(InstType::JMP, 0);
            auto jmp_to_test = _gen_ist_off;
            uint32_t len = 1;

            auto stmt = _stmt->generate({ param._level, param._offset + len,
                                          param._slot, param._ret });
            if (stmt._len == 0) {
                _gen_pop;
                return _gen_ret(0);
            }
            len += stmt._len;
            _gen_ist(jmp_to_test).set_op1(param._offset + len);

            auto cond = _cond->generate(param);
            if (cond._len == 0) {
                _gen_popn(len);
                return _gen_ret(0);
            }
            _gen_ist1(_make_jn(_cond->get_op()), param._offset + len);
            len += cond._len + 1;

            // handle breaks and continues
            for (const auto jmp_to_end: stmt._breaks)
                _gen_ist(jmp_to_end).set_op1(param._offset + len);
            for (const auto jmp_to_head: stmt._continues)
                _gen_ist(jmp_to_head).set_op1(param._offset);

            return _gen_ret(len);
        }
    };

    class AssignAST final: public ExprAST, public StmtAST {
    private:
        _ptr<IdExprAST> _id;
        _ptr<ExprAST> _value;

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

        _GenResult generate(_GenParam param) override {

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

        _GenResult generate(_GenParam param) override {

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
             *     ...cond
             *     j$(op) Loop
             * .End
             */
            auto stmt = _stmt->generate(param);
            if (stmt._len == 0)
                return _gen_ret(0);
            auto len = stmt._len;

            auto cond = _cond->generate(param);
            if (cond._len == 0) {
                _gen_popn(len);
                return _gen_ret(0);
            }

            _gen_ist1(_make_jmp(_cond->get_op()), param._offset);
            len += cond._len + 1;

            for (const auto jmp_to_end: stmt._breaks)
                _gen_ist(jmp_to_end).set_op1(param._offset + len);
            for (const auto jmp_to_head: stmt._continues)
                _gen_ist(jmp_to_head).set_op1(param._offset);

            return _gen_ret(len);
        }
    };
}

#endif //C0_LOOP_STMT_H
