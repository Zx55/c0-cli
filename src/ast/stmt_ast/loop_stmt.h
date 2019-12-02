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
        explicit WhileStmtAST(_ptr<CondExprAST> cond, _ptr<StmtAST> stmt):
            _cond(std::move(cond)), _stmt(std::move(stmt)) { }

        void graphize(std::ostream& out, int t) override {
            out << "<while-stmt>\n" << _mid(t);
            _cond->graphize(out, t + 1);
            out << _end(t);
            _stmt->graphize(out, t + 1);
        }
    };

    class AssignAST final: public ExprAST, public StmtAST {
    private:
        _ptr<IdExprAST> _id;
        _ptr<ExprAST> _value;

    public:
        explicit AssignAST(_ptr<IdExprAST> id, _ptr<ExprAST> value):
                _id(std::move(id)), _value(std::move(value)) { }

        void graphize(std::ostream& out, int t) override {
            out << "<assignment>\n" << _mid(t);
            _id->graphize(out, t + 1);
            out << _end(t);
            _value->graphize(out, t + 1);
        }
    };

    class ForStmtAST final: public StmtAST {
    private:
        _ptrs<AssignAST> _init;
        _ptr<CondExprAST> _cond;
        _ptrs<ExprAST> _update;
        _ptr<StmtAST> _stmt;

    public:
        explicit ForStmtAST(_ptrs<AssignAST> init, _ptr<CondExprAST> cond, _ptrs<ExprAST> update, _ptr<StmtAST> stmt):
            _init(std::move(init)), _cond(std::move(cond)), _update(std::move(update)), _stmt(std::move(stmt)) { }

        void graphize(std::ostream& out, int t) override {
            out << "<for-stmt>\n";

            if (!_init.empty())
                out << ((_cond == nullptr && _update.empty() && _stmt == nullptr) ? _end(t) : _mid(t))
                    << "<init-stmt>\n";
            graphize_list(_init, out, t + 1, t + 2);

            if (_cond != nullptr) {
                out << ((_update.empty() && _stmt == nullptr) ? _end(t) : _mid(t)) << "<for-cond>";
                _cond->graphize(out, t + 1);
            }

            if (!_update.empty())
                out << (_stmt == nullptr ? _end(t) : _mid(t)) << "<for-update>\n";
            graphize_list(_update, out, t + 1, t + 2);

            if (_stmt != nullptr) {
                out << _end(t);
                _stmt->graphize(out, t + 1);
            }
        }
    };

    class DoWhileStmtAST final: public StmtAST {
    private:
        _ptr<CondExprAST> _cond;
        _ptr<StmtAST> _stmt;

    public:
        explicit DoWhileStmtAST(_ptr<CondExprAST> cond, _ptr<StmtAST> stmt):
            _cond(std::move(cond)), _stmt(std::move(stmt)) { }

        void graphize(std::ostream& out, int t) override {
            out << "<do-while-stmt>\n" << _mid(t);
            _cond->graphize(out, t + 1);
            out << _end(t);
            _stmt->graphize(out, t + 1);
        }
    };
}

#endif //C0_LOOP_STMT_H
