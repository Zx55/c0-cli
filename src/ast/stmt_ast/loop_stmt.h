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

    class ForStmtAST final: public StmtAST {
    private:
        _ptrs<ExprAST> _init;
        _ptr<CondExprAST> _cond;
        _ptr<StmtAST> _stmt;

    public:
        explicit ForStmtAST(_ptrs<ExprAST> init, _ptr<CondExprAST> cond, _ptr<StmtAST> stmt):
            _init(std::move(init)), _cond(std::move(cond)), _stmt(std::move(stmt)) { }

        void graphize(std::ostream& out, int t) override {
            out << "<for-stmt>\n";

            if (!_init.empty()) {
                if (_cond == nullptr && _stmt == nullptr) out << _end(t);
                else out << _mid(t);
                out << "<for-init-stmt>\n";
            }
            graphize_list(_init, out, t + 1, t + 2);

            if (_cond != nullptr) {
                if (_stmt == nullptr) out << _end(t);
                else out << _mid(t);
                out << "<for-cond-expr>\n";
                _cond->graphize(out, t + 1);
            }

            if (_stmt != nullptr) {
                out << _end(t) << "<for-update-stmt>\n";
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
