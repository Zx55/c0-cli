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

        _GenResult generate(_GenParam param) override {

        }
    };

    class AssignAST final: public ExprAST, public StmtAST {
    private:
        _ptr<IdExprAST> _id;
        _ptr<ExprAST> _value;

    public:
        explicit AssignAST(range_t range, _ptr<IdExprAST> id, _ptr<ExprAST> value):
            ExprAST(range), StmtAST(range), _id(std::move(id)), _value(std::move(value)) { }

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

        _GenResult generate(_GenParam param) override {

        }
    };
}

#endif //C0_LOOP_STMT_H
