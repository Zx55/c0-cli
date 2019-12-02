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
        explicit IfElseStmtAST(_ptr<CondExprAST> cond, _ptr<StmtAST> s_true, _ptr<StmtAST> s_false):
            _cond(std::move(cond)), _true(std::move(s_true)), _false(std::move(s_false)) { }

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
    };

    class LabelStmtAST final: public StmtAST {
    private:
        _ptr<ExprAST> _case;
        _ptr<StmtAST> _stmt;

    public:
        explicit LabelStmtAST(_ptr<ExprAST> e_case, _ptr<StmtAST> stmt):
            _case(std::move(e_case)), _stmt(std::move(stmt)) { }

        void graphize(std::ostream& out, int t) override {
            out << "[case] ";
            _case->graphize(out, t + 1);
            out << _end(t);
            _stmt->graphize(out, t + 1);
        }
    };

    class SwitchStmtAST final: public StmtAST {
    private:
        _ptr<ExprAST> _cond;
        _ptrs<LabelStmtAST> _cases;
        _ptr<StmtAST> _default;

    public:
        explicit SwitchStmtAST(_ptr<ExprAST> cond, _ptrs<LabelStmtAST> cases, _ptr<StmtAST> s_default):
            _cond(std::move(cond)), _cases(std::move(cases)), _default(std::move(s_default)) { }

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
    };
}

#endif //C0_COND_STMT_H
