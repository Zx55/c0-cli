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

namespace cc0 {
    class ConstSym final {
        friend inline void swap(ConstSym& lhs, ConstSym& rhs) {
            std::swap(lhs._index, rhs._index);
            std::swap(lhs._type, rhs._type);
            std::swap(lhs._value, rhs._value);
        }

    private:
        uint32_t _index;        // index in constants table
        Type _type;
        std::any _value;

    public:
        struct _Hash {
            std::size_t operator() (const ConstSym& sym) const {
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

        ConstSym(uint32_t index, Type type, std::any value):
            _index(index), _type(type), _value(std::move(value)) { }
        ConstSym(const ConstSym&) = default;
        ConstSym(ConstSym&&) = default;
        ConstSym& operator=(ConstSym rhs) {
            swap(*this, rhs);
            return *this;
        }
        bool operator==(const ConstSym& rhs) const {
            if (_type != rhs._type) return false;

            switch (_type) {
                case Type::INT:
                    return std::any_cast<int32_t>(_value) == std::any_cast<int32_t>(rhs._value);
                case Type::DOUBLE:
                    return std::any_cast<double>(_value) == std::any_cast<double>(rhs._value);
                case Type::STRING:
                    return std::any_cast<std::string>(_value) == std::any_cast<std::string>(rhs._value);
                default:
                    return false;
            }
        }
    };

    class FuncSym final {
        friend inline void swap(FuncSym &lhs, FuncSym &rhs) {
            std::swap(lhs._index, rhs._index);
            std::swap(lhs._id, rhs._id);
            std::swap(lhs._ret, rhs._ret);
            std::swap(lhs._params, rhs._params);
            std::swap(lhs._code_start, rhs._code_start);
        }

    private:
        uint32_t _index;        // index in functions table
        std::string _id;
        Type _ret;
        std::unordered_map<std::string, Type> _params;
        
        uint32_t _code_start;   // index in instructions
        [[maybe_unused]] uint32_t _usage;

    public:
        FuncSym(uint32_t index, std::string id, Type ret, uint32_t start):
            _index(index), _id(std::move(id)), _ret(ret), _code_start(start), _usage(0) { }
        FuncSym(const FuncSym&) = default;
        FuncSym(FuncSym&&) = default;
        FuncSym& operator=(FuncSym rhs) {
            swap(*this, rhs);
            return *this;
        }
        bool operator==(const FuncSym& rhs) { return _id == rhs._id && _params == rhs._params; }

        [[nodiscard]] inline bool put_param(std::string param, Type type) {
            if (auto it = _params.find(param); it != _params.end())
                return false;
            _params.insert({ std::move(param), type });
            return true;
        }
    };

    class VarSym final {
        friend inline void swap(VarSym& lhs, VarSym& rhs) {
            std::swap(lhs._id, rhs._id);
            std::swap(lhs._type, rhs._type);
            std::swap(lhs._value, rhs._value);
            std::swap(lhs._level, rhs._level);
        }

    private:
        std::string _id;
        Type _type;
        std::optional<std::any> _value;
        bool _const;

        uint32_t _addr;         // address in vm
        uint32_t _level;
        [[maybe_unused]] uint32_t _usage;

    public:
        VarSym(std::string id, Type type, std::any value, bool f_const, uint32_t level):
            _id(std::move(id)), _type(type), _value(std::move(value)), _const(f_const),
            _addr(0), _level(level), _usage(0) { }
        VarSym(const VarSym&) = default;
        VarSym(VarSym&&) = default;
        VarSym& operator=(VarSym rhs) {
            swap(*this, rhs);
            return *this;
        }
        bool operator==(const VarSym& rhs) { return _id == rhs._id && _level == rhs._level; }

        [[nodiscard]] inline bool is_init() const { return _value == std::nullopt; }
        [[nodiscard]] inline bool is_const() const { return _const; }

        [[nodiscard]] inline Type get_type() const { return _type; }
        [[nodiscard]] inline uint32_t get_addr() const { return _addr; }

        inline void set_value(std::any value) { _value = std::move(value); }
    };
}

#endif //C0_SYMBOL_H
