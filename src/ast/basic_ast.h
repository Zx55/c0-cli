/*
 * Created by zx5 on 2019/11/26.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_BASIC_AST_H
#define C0_BASIC_AST_H

#include "tools/alias.h"
#include "tools/enums.h"
#include "tools/error.h"
#include "ctx/gen_ctx.h"

#include "lexer/token.h"
#include "ist/instruction.h"

#include <vector>
#include <string>
#include <memory>
#include <optional>
#include <any>
#include <utility>

#define _symtbl GeneratorContext::get_tbl()
#define _gen_err(_code) GeneratorContext::put_fatal(C0Err(_code, AST::_range.first, AST::_range.second))
#define _gen_wrn(_code) GeneratorContext::put_wrn(C0Err(_code, AST::_range.first, AST::_range.second))
#define _gen_err2(_code) GeneratorContext::put_fatal(C0Err(_code, ExprAST::_range.first, ExprAST::_range.second))
#define _gen_wrn2(_code) GeneratorContext::put_wrn(C0Err(_code, ExprAST::_range.first, ExprAST::_range.second))

#define _gen_ist0(_type) GeneratorContext::put_ist(Instruction(_type))
#define _gen_ist1(_type, _op1) GeneratorContext::put_ist(Instruction(_type, _op1))
#define _gen_ist2(_type, _op1, _op2) GeneratorContext::put_ist(Instruction(_type, _op1, _op2))
#define _gen_pop GeneratorContext::pop_ist()
#define _gen_popn(n) GeneratorContext::pop_ist(n)
// get instruction based on _off
#define _gen_ist(_off) GeneratorContext::get_ist(_off)
// global offset
#define _gen_ist_off static_cast<uint32_t>(GeneratorContext::get_ist().size())

#define _gen_ret(_len) { _len, {}, {} }

#define _gen_move_back(_src, _res) _src.insert(_src.end(), std::make_move_iterator(_res.begin()), \
    std::make_move_iterator(_res.end()))

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
                case Type::VOID:    return "void";
                case Type::INT:     return "int32";
                case Type::CHAR:    return "char";
                case Type::STRING:  return "string";
                case Type::DOUBLE:  return "float64";
                default:            return "undefined";
            }
        }

        [[nodiscard]] inline static std::string _op_str(Op op) {
            switch (op) {
                case Op::ADD:   return "+";
                case Op::SUB:   return "-";
                case Op::MUL:   return "*";
                case Op::DIV:   return "/";
                case Op::LT:    return "<";
                case Op::LE:    return "<=";
                case Op::GT:    return ">";
                case Op::GE:    return ">=";
                case Op::EQ:    return "==";
                case Op::NEQ:   return "!=";
                default:        return "undefined";
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

        [[nodiscard]] inline static uint32_t _make_slot(Type type) {
            switch (type) {
                case Type::INT:     [[fallthrough]];
                case Type::CHAR:    return 1;
                case Type::DOUBLE:  return 2;
                default:            return -1;
            }
        }

        [[nodiscard]] inline static InstType _make_jmp(Op op) {
            switch (op) {
                case Op::GT:    return InstType::JG;
                case Op::GE:    return InstType::JGE;
                case Op::LT:    return InstType::JL;
                case Op::LE:    return InstType::JLE;
                case Op::EQ:    return InstType::JE;
                case Op::NEQ:   return InstType::JNE;
                default:        return InstType::NOP;
            }
        }

        [[nodiscard]] inline static InstType _make_jn(Op op) {
            switch (op) {
                case Op::GT:    return InstType::JLE;
                case Op::GE:    return InstType::JL;
                case Op::LT:    return InstType::JGE;
                case Op::LE:    return InstType::JG;
                case Op::EQ:    return InstType::JNE;
                case Op::NEQ:   return InstType::JE;
                default:        return InstType::NOP;
            }
        }

        struct _GenParam final {
            /*
             * scope info
             * global - 0, local - 1, 2, ...
             */
            uint32_t _level;

            /*
             * position info
             * _offset - offset in current function's instructions list
             *           we use this value in <stmt> to make jmp instruction (function offset)
             *              in a new function block, it will be 0
             * _slot   - offset in current frame in vm
             *           we use this value to fill symbol table
             *           in a new function, it will be slots(parameters)
             */
            uint32_t _offset;
            uint32_t _slot;

            /*
             * 1. this field is for <return-stmt>'s type check
             * 2. also we use this field to notify <id> to generate an address or a value
             */
            Type _ret;

            /*
             * is stmt? we use this field to pop <function-call>'s return value in <stmt> and <for-update>
             */
            bool _stmt;

            _GenParam(uint32_t level, uint32_t offset, uint32_t slot,
                    Type type = Type::UNDEFINED, bool f_stmt = false):
                _level(level), _offset(offset), _slot(slot), _ret(type), _stmt(f_stmt) { }
        };

        struct _GenResult final {
            /*
             * instructions info
             * it seems all ast will generate more than one instruction (empty-stmt => nop)
             * so _len == 0 means some errors occur
             */
            uint32_t _len;

            /*
             * back-fill info
             * they store offset in global instructions list
             * we set backfill info base on global offset
             */
            std::vector<uint32_t> _breaks;
            std::vector<uint32_t> _continues;

        public:
            _GenResult(uint32_t len, std::vector<uint32_t> breaks, std::vector<uint32_t> continues):
                    _len(len), _breaks(std::move(breaks)), _continues(std::move(continues)) { }
        };

        range_t _range;

    public:
        explicit AST(range_t range): _range(std::move(range)) { }

        [[nodiscard]] inline range_t get_range() const { return _range; }

        virtual void graphize(std::ostream& out, int t) = 0;

        [[nodiscard]] virtual _GenResult generate(_GenParam param) = 0;
    };
}

#endif //C0_BASIC_AST_H
