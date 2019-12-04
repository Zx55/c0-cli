/*
 * Created by zx5 on 2019/12/1.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_SYMBOL_H
#define C0_SYMBOL_H

#include "tools/alias.h"
#include "tools/enums.h"

#include <any>
#include <optional>
#include <unordered_map>
#include <utility>

namespace cc0::symbol {
    class ConsSym final {
        friend inline void swap(ConsSym& lhs, ConsSym& rhs) {
            std::swap(lhs._type, rhs._type);
            std::swap(lhs._value, rhs._value);
        }

    private:
        Type _type;
        std::any _value;

    public:
        struct _Hash final {
            std::size_t operator() (const ConsSym& sym) const {
                std::string str = std::to_string(static_cast<int32_t>(sym._type));
                switch (sym._type) {
                    case Type::INT:
                        str += std::to_string(std::any_cast<int32_t>(sym._value));
                    case Type::DOUBLE:
                        str += std::to_string(std::any_cast<double>(sym._value));
                    case Type::STRING:
                        str += std::any_cast<std::string>(sym._value);
                    default:
                        str += "undefined";
                }
                return std::hash<std::string>{}(str);
            }
        };

        ConsSym(Type type, std::any value): _type(type), _value(std::move(value)) { }
        ConsSym(const ConsSym&) = default;
        ConsSym(ConsSym&&) = default;
        ConsSym& operator=(ConsSym rhs) {
            swap(*this, rhs);
            return *this;
        }
        bool operator==(const ConsSym& rhs) const {
            if (_type != rhs._type) return false;

            try {
                switch (_type) {
                    case cc0::Type::INT:
                        return std::any_cast<int32_t>(_value) == std::any_cast<int32_t>(rhs._value);
                    case cc0::Type::DOUBLE:
                        return std::any_cast<double>(_value) == std::any_cast<double>(rhs._value);
                    case cc0::Type::STRING:
                        return std::any_cast<std::string>(_value) == std::any_cast<std::string>(rhs._value);
                    default:
                        return false;
                }
            } catch (const std::bad_any_cast&) {
                return false;
            }
        }
    };

    class FuncSym final {
        friend inline void swap(FuncSym& lhs, FuncSym& rhs) {
            std::swap(lhs._index, rhs._index);
            std::swap(lhs._id, rhs._id);
            std::swap(lhs._ret, rhs._ret);
            std::swap(lhs._params, rhs._params);
            std::swap(lhs._offset, rhs._offset);
            std::swap(lhs._usage, rhs._usage);
        }

    private:
        uint32_t _index;        // index in functions table

        /*
         * function info
         */
        std::string _id;
        Type _ret;
        std::unordered_map<std::string, std::pair<Type, bool>> _params;

        /*
         * runtime info
         * _offset - index of first assembly in instructions
         */
        uint32_t _offset;

        uint32_t _usage;

    public:
        // get offset in ast
        FuncSym(uint32_t index, std::string id, Type ret, uint32_t offset):
                _index(index), _id(std::move(id)), _ret(ret), _offset(offset), _usage(0) { }
        FuncSym(const FuncSym&) = default;
        FuncSym(FuncSym&&) = default;
        FuncSym& operator=(FuncSym rhs) {
            swap(*this, rhs);
            return *this;
        }
        bool operator==(const FuncSym& rhs) { return _id == rhs._id && _params == rhs._params; }

        [[nodiscard]] inline auto& get_params() const { return _params; }

        [[nodiscard]] inline bool put_param(std::string param, Type type, bool f_const = false) {
            if (auto it = _params.find(param); it != _params.end())
                return false;
            _params.insert({ std::move(param), { type, f_const } });
            return true;
        }
    };

    class VarSym final {
        friend inline void swap(VarSym& lhs, VarSym& rhs) {
            std::swap(lhs._id, rhs._id);
            std::swap(lhs._type, rhs._type);
            std::swap(lhs._init, rhs._init);
            std::swap(lhs._const, rhs._const);
            std::swap(lhs._offset, rhs._offset);
            std::swap(lhs._level, rhs._level);
            std::swap(lhs._usage, rhs._usage);
        }

    private:
        /*
         * variable info
         * _id    - identifier
         * _type  - variable type
         * _init  - is init?
         * _const - is const?
         */
        std::string _id;
        Type _type;
        bool _init;
        bool _const;

        /*
         * runtime info
         * _offset - offset in current/global frame in vm stack.
         *
         */
        uint32_t _offset;

        /*
         * scope info
         * _level  - index of block
         *           when leaving a block, we should pop vars of this block(_level).
         *
         * for example
         *      global:
         *          level - 0:
         *              var a ...
         *              var b ...
         *      local:
         *          level - 0:
         *              var c ...
         *          level - 1:    // like a if-else-stmt block
         *              var c ... // override definition in level 0.
         *          ...
         *
         * we will destroy domain 1 when a function is end
         * and we leave if-else-block, level 1 of domain 1 will be destroyed
         */
        uint32_t _level;

        uint32_t _usage;

    public:
        VarSym(std::string id, Type type, bool init, bool f_const, uint32_t offset, uint32_t level):
                _id(std::move(id)), _type(type), _init(init), _const(f_const),
                _offset(offset), _level(level), _usage(0) { }
        VarSym(const VarSym&) = default;
        VarSym(VarSym&&) = default;
        VarSym& operator=(VarSym rhs) {
            swap(*this, rhs);
            return *this;
        }
        bool operator==(const VarSym& rhs) const {
            return _id == rhs._id && _level == rhs._level;
        }

        [[nodiscard]] inline bool is_init() const { return _init; }
        [[nodiscard]] inline bool is_const() const { return _const; }

        [[nodiscard]] inline std::string get_id() const { return _id; }
        [[nodiscard]] inline Type get_type() const { return _type; }
        [[nodiscard]] inline uint32_t get_offset() const { return _offset; }
        [[nodiscard]] inline uint32_t get_level() const { return _level; }

        inline void init() { _init = true; }
    };
}

#endif //C0_SYMBOL_H
