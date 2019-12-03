/*
 * Created by zx5 on 2019/11/26.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_BASIC_AST_H
#define C0_BASIC_AST_H

#include "tools/alias.h"
#include "tools/enums.h"
#include "tools/error.h"

#include "lexer/token.h"

#include <vector>
#include <string>
#include <memory>
#include <optional>
#include <any>
#include <utility>

namespace cc0::ast {
    class AST {
    protected:
        [[nodiscard]] inline static std::string _mid(int t) {
            return std::string(4 * t, ' ') + "├── ";
        }
        [[nodiscard]] inline static std::string _end(int t) {
            return std::string(4 * t, ' ') + "└── ";
        }
        [[nodiscard]] inline static std::string _type_str(Type type) {
            switch (type) {
                case Type::VOID: return "void";
                case Type::INT: return "int32";
                case Type::CHAR: return "char";
                case Type::STRING: return "string";
                case Type::DOUBLE: return "float64";
                default: return "undefined";
            }
        }
        [[nodiscard]] inline static std::string _op_str(Op op) {
            switch (op) {
                case Op::ADD: return "+";
                case Op::SUB: return "-";
                case Op::MUL: return "*";
                case Op::DIV: return "/";
                case Op::LT: return "<";
                case Op::LE: return "<=";
                case Op::GT: return ">";
                case Op::GE: return ">=";
                case Op::EQ: return "==";
                case Op::NEQ: return "!=";
                default: return "undefined";
            }
        }
        template <typename T> static void _graphize_list(const _ptrs<T>& list, std::ostream& out, int tab, int child) {
            if (!list.empty()) {
                for (auto it = list.cbegin(); it != list.cend() - 1; ++it) {
                    out << _mid(tab);
                    (*it)->graphize(out, child);
                }
                out << _end(tab);
                (*(list.cend() - 1))->graphize(out, child);
            }
        }

        struct _GenParam {
            uint32_t _domain;
            uint32_t _level;
            uint32_t _slot;
            Type _ret;

            explicit _GenParam(uint32_t domain, uint32_t level, uint32_t slot, Type type = Type::UNDEFINED):
                _domain(domain), _level(level), _slot(slot), _ret(type) { }
        };

        struct _GenResult {
            uint32_t _offset;
            uint32_t _len;

            std::vector<uint32_t> _breaks;
            std::vector<uint32_t> _continues;
        };

        range_t _range;

    public:
        explicit AST(range_t range): _range(std::move(range)) { }

        [[nodiscard]] inline range_t get_range() const { return _range; }

        // virtual _GenResult generate(_GenParam param) = 0;
        virtual void graphize(std::ostream& out, int t) = 0;
    };
}

#endif //C0_BASIC_AST_H
