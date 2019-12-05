/*
 * Created by zx5 on 2019/11/28.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_COND_STMT_H
#define C0_COND_STMT_H

#include "basic_stmt.h"

namespace cc0::ast {
    class IfElseStmtAST final: public StmtAST {
    private:
        _ptr<CondExprAST> _cond;
        _ptr<StmtAST> _true;
        _ptr<StmtAST> _false;

    public:
        explicit IfElseStmtAST(range_t range, _ptr<CondExprAST> cond, _ptr<StmtAST> s_true, _ptr<StmtAST> s_false):
            StmtAST(range), _cond(std::move(cond)), _true(std::move(s_true)), _false(std::move(s_false)) { }

        void graphize(std::ostream& out, int t) override {
            out << "<if-else-stmt>\n" << _mid(t);
            _cond->graphize(out, t + 1);
            if (_false == nullptr) out << _end(t);
            else out << _mid(t);
            _true->graphize(out, t + 1);
            if (_false != nullptr) {
                out << _end(t);
                _false->graphize(out, t + 1);
            }
        }

        [[nodiscard]] _GenResult generate(_GenParam param) override {
            auto cond = _cond->generate(param);
            auto len = cond._len;
            if (len == 0)
                return _gen_ret(0);

            auto jmp_to_false = _gen_ist_off;
            _gen_ist1(_make_jn(_cond->get_op()), 0);
            ++len;

            auto true_stmt = _true->generate({ param._level, param._offset + len,
                                               param._slot, param._ret });
            if (true_stmt._len == 0) {
                _gen_popn(len);
                return _gen_ret(0);
            }
            len += true_stmt._len;

            if (_false == nullptr) {
                /*
                 * `if (cond) stmt` will generate:
                 *
                 *     ...cond
                 *     j$(!op) .End
                 *     ...stmt
                 * .End
                 *
                 * and the .End here is false offset
                 */
                _gen_ist(jmp_to_false).set_op1(param._offset + len);
                return { len + 1, std::move(true_stmt._breaks), std::move(true_stmt._continues) };
            }

            /*
             * `if (cond) stmt1 else stmt2` will generate:
             *
             *     ...cond
             *     j$(!op) .False
             *     ...stmt1
             *     jmp .End
             * .False
             *     ...stmt2
             * .End
             */
            auto jmp_to_end = _gen_ist_off;
            _gen_ist1(InstType::JMP, 0);
            ++len;
            _gen_ist(jmp_to_false).set_op1(param._offset + len);

            auto false_stmt = _false->generate({ param._level, param._offset + len,
                                                 param._slot, param._ret });
            if (false_stmt._len == 0) {
                _gen_popn(len);
                return _gen_ret(0);
            }
            len += false_stmt._len;

            _gen_ist(jmp_to_end).set_op1(param._offset + len);

            auto breaks = std::move(true_stmt._breaks);
            auto continues = std::move(true_stmt._continues);
            _gen_move_back(breaks, false_stmt._breaks);
            _gen_move_back(continues, false_stmt._continues);
            return { len + 1, std::move(breaks), std::move(continues) };
        }
    };

    class LabelStmtAST final: public StmtAST {
    private:
        _ptr<ExprAST> _case;
        _ptr<StmtAST> _stmt;

    public:
        explicit LabelStmtAST(range_t range, _ptr<ExprAST> e_case, _ptr<StmtAST> stmt):
            StmtAST(range), _case(std::move(e_case)), _stmt(std::move(stmt)) { }

        void graphize(std::ostream& out, int t) override {
            out << "[case] ";
            _case->graphize(out, t + 1);
            out << _end(t);
            _stmt->graphize(out, t + 1);
        }

        _GenResult generate(_GenParam param) override {

        }
    };

    class SwitchStmtAST final: public StmtAST {
    private:
        _ptr<ExprAST> _cond;
        _ptrs<LabelStmtAST> _cases;
        _ptr<StmtAST> _default;

    public:
        explicit SwitchStmtAST(range_t range, _ptr<ExprAST> cond, _ptrs<LabelStmtAST> cases, _ptr<StmtAST> s_default):
            StmtAST(range), _cond(std::move(cond)), _cases(std::move(cases)), _default(std::move(s_default)) { }

        void graphize(std::ostream& out, int t) override {
            out << "<switch-stmt>\n" << ((_cases.empty() && _default == nullptr) ? _end(t) : _mid(t)) << "[cond] ";
            _cond->graphize(out, t + 1);
            if (!_cases.empty()) {
                for (auto it = _cases.cbegin(); it != _cases.cend() - 1; ++it) {
                    out << _mid(t);
                    (*it)->graphize(out, t + 1);
                }
                out << (_default == nullptr ? _end(t) : _mid(t));
                (*(_cases.cend() - 1))->graphize(out, t + 1);
            }
            if (_default != nullptr) {
                out << _end(t) << "[default] ";
                _default->graphize(out, t + 1);
            }
        }

        _GenResult generate(_GenParam param) override {

        }
    };
}

#endif //C0_COND_STMT_H
