/*
 * Created by zx5 on 2019/12/1.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_SYMBOL_H
#define C0_SYMBOL_H

#include "tools/alias.h"
#include "tools/enums.h"
#include "tools/utils.h"

#include <vector>
#include <any>
#include <optional>
#include <map>
#include <unordered_map>
#include <utility>

namespace cc0::symbol {
    template <typename _Key, typename _Value, typename _Hash = std::hash<_Key>>
    class LinkedHashMap final {
        friend inline void swap(LinkedHashMap& lhs, LinkedHashMap& rhs) {
            std::swap(lhs._map, rhs._map);
            std::swap(lhs._list, rhs._list);
        }

        friend class SymTbl;

    private:
        std::unordered_map<_Key, _Value, _Hash> _map;
        std::vector<_Key> _list;

        auto& get_map() {
            return _map;
        }

        auto& get_list() {
            return _list;
        }

    public:
        LinkedHashMap() = default;
        bool operator==(const LinkedHashMap& rhs) const {
            return _list == rhs._list && _map == rhs._map;
        }

        inline auto begin() const { return _list.begin(); }
        inline auto end() const { return _list.end(); }

        void insert(const _Key& key, const _Value& value) {
            if (auto it = _map.find(key); it == _map.end())
                _list.push_back(key);
            _map[key] = value;
        }

        inline void insert(const std::pair<_Key, _Value>& p) {
            if (auto it = _map.find(p.first); it == _map.end())
                _list.push_back(p.first);
            _map[p.first] = p.second;
        }

        inline auto find(_Key key) const {
            return _map.find(key);
        }

        inline auto find_end() const {
            return _map.end();
        }

        inline auto index(_Key key) const {
            int32_t ret = -1;

            for (auto it = begin(); it != end(); ++it) {
                if (*it == key) {
                    ret = it - begin();
                    break;
                }
            }

            return ret;
        }

        auto at(uint32_t index) const {
            auto key = _list.at(index);
            return find(key);
        }

        auto size() const {
            return _list.size();
        }

        auto empty() const {
            return _list.empty();
        }

        void clear() {
            _map.clear();
            _list.clear();
        }
    };

    class ConsSym final {
        friend inline void swap(ConsSym& lhs, ConsSym& rhs) {
            std::swap(lhs._type, rhs._type);
            std::swap(lhs._value, rhs._value);
        }

        friend std::ostream& operator<<(std::ostream& out, const ConsSym& rhs) {
            switch (rhs._type) {
                case Type::DOUBLE:
                    out << "D " << std::any_cast<double>(rhs._value);
                    break;
                case ast::Type::INT:
                    out << "I " << std::any_cast<int32_t>(rhs._value);
                    break;
                case ast::Type::STRING:
                    out << "S " << std::any_cast<std::string>(rhs._value);
                    break;
                default:
                    out << "U";
            }

            return out;
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
                        break;
                    case Type::DOUBLE:
                        str += std::to_string(std::any_cast<double>(sym._value));
                        break;
                    case Type::STRING:
                        str += std::any_cast<std::string>(sym._value);
                        break;
                    default:
                        str += "undefined";
                }
                return std::hash<std::string>{}(str);
            }
        };

        ConsSym(Type type, std::any value):
            _type(type), _value(std::move(value)) { }
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

        inline void output_bin(std::ostream& out) const {
            switch (_type) {
                case Type::STRING: {
                    auto str_value = std::any_cast<std::string>(_value);

                    uint8_t type = utils::swap_endian(static_cast<uint8_t >(0x00));
                    out.write(reinterpret_cast<char*>(&type), 1);

                    uint16_t length = utils::swap_endian(static_cast<uint16_t>(str_value.size()));
                    out.write(reinterpret_cast<char*>(&length), 2);

                    for (const auto ch: str_value)
                        out.write(&ch, 1);
                    break;
                }
                case Type::INT: {
                    uint8_t type = utils::swap_endian(static_cast<uint8_t >(0x01));
                    out.write(reinterpret_cast<char*>(&type), 1);

                    auto int_value = std::any_cast<int32_t>(_value);
                    out.write(reinterpret_cast<char*>(&int_value), 4);

                    break;
                }
                case Type::DOUBLE: {
                    uint8_t type = utils::swap_endian(static_cast<uint8_t>(0x02));
                    out.write(reinterpret_cast<char*>(&type), 1);

                    auto float_value = std::any_cast<double>(_value);
                    uint64_t bits = *reinterpret_cast<uint64_t*>(&float_value);
                    uint32_t high = utils::swap_endian(static_cast<uint32_t>((bits & 0xffffffff00000000) >> 32u));
                    uint32_t low = utils::swap_endian(static_cast<uint32_t>(bits & 0x00000000ffffffff));
                    out.write(reinterpret_cast<char*>(&high), 4);
                    out.write(reinterpret_cast<char*>(&low), 4);

                    break;
                }
                default:
                    return;
            }
        }
    };

    class FuncSym final {
        friend inline void swap(FuncSym& lhs, FuncSym& rhs) {
            std::swap(lhs._id, rhs._id);
            std::swap(lhs._ret, rhs._ret);
            std::swap(lhs._params, rhs._params);
            std::swap(lhs._offset, rhs._offset);
            std::swap(lhs._usage, rhs._usage);
        }

    private:
        /*
         * function info
         */
        std::string _id;
        Type _ret;
        LinkedHashMap<std::string, std::pair<Type, bool>> _params;
        uint32_t _slot;

        /*
         * runtime info
         * _offset - index of first assembly in instructions
         */
        uint32_t _offset;

        uint32_t _usage;

        [[nodiscard]] inline static uint32_t _make_slot(Type type) {
            switch (type) {
                case Type::INT:     [[fallthrough]];
                case Type::CHAR:    return 1;
                case Type::DOUBLE:  return 2;
                default:            return -1;
            }
        }

    public:
        FuncSym(std::string id, Type ret, uint32_t offset):
                _id(std::move(id)), _ret(ret), _slot(0), _offset(offset), _usage(0) { }
        FuncSym(const FuncSym&) = default;
        FuncSym(FuncSym&&) = default;
        FuncSym& operator=(FuncSym rhs) {
            swap(*this, rhs);
            return *this;
        }
        bool operator==(const FuncSym& rhs) { return _id == rhs._id && _params == rhs._params; }

        [[nodiscard]] inline auto& get_params() const { return _params; }
        [[nodiscard]] inline Type get_ret_type() const { return _ret; }
        [[nodiscard]] inline uint32_t get_offset() const { return _offset; }
        [[nodiscard]] inline uint32_t get_slot() const { return _slot; }

        [[nodiscard]] inline bool put_param(const std::string& param, Type type, bool f_const = false) {
            if (auto it = _params.find(param); it != _params.find_end())
                return false;
            _slot += _make_slot(type);
            _params.insert(param, { type, f_const });
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
         *          level - 1:
         *              function name
         *              params
         *              var c ...
         *          level - 2:    // like a if-else-stmt block
         *              var c ... // override definition in level 0.
         *          ...
         *
         * we will destroy local symtbl when a function is end
         * and we leave if-else-block, level 2 will be destroyed
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
        [[nodiscard]] inline bool is_glob() const { return _level == 0; }

        [[nodiscard]] inline std::string get_id() const { return _id; }
        [[nodiscard]] inline Type get_type() const { return _type; }
        [[nodiscard]] inline uint32_t get_offset() const { return _offset; }
        [[nodiscard]] inline uint32_t get_level() const { return _level; }

        inline void init() { _init = true; }
    };
}

#endif //C0_SYMBOL_H
