/*
 * Created by zx5 on 2019/12/1.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_SYMTBL_H
#define C0_SYMTBL_H

#include "symbol.h"

namespace cc0::symbol {
    class SymTbl final {
    private:
        std::unordered_map<ConsSym, uint32_t, ConsSym::_Hash> _cons;
        std::unordered_map<std::string, FuncSym> _funcs;

        /*
         * variable table
         * _globs - store global vars (_domain = 0)
         *          storing vars in two table is to make finding global vars easy.
         * _local  - store local vars  (_domain > 0)
         *          LinkedHashMap will be better here
         */
        std::unordered_map<std::string, VarSym> _globs;
        std::vector<VarSym> _local;

    public:
        SymTbl() = default;
        SymTbl(const SymTbl&) = delete;
        SymTbl(SymTbl&&) = delete;
        SymTbl& operator=(SymTbl tbl) = delete;

        inline void init() {
            _cons.clear();
            _funcs.clear();
            _globs.clear();
            _local.clear();
        }

        // when we use an identifier
        [[nodiscard]] inline std::optional<VarSym> get_var(const std::string& id) const {
            for (auto it = _local.crbegin(); it != _local.crend(); ++it)
                if (it->get_id() == id) return *it;
            if (auto it = _globs.find(id); it != _globs.end())
                return it->second;
            return std::nullopt;
        }

        // when we declare an identifier
        [[nodiscard]] inline std::optional<VarSym> get_var(const std::string& id, uint32_t level) const {
            for (auto it = _local.crbegin(); it != _local.crend() && it->get_level() == level; ++it)
                if (it->get_id() == id) return *it;
            return std::nullopt;
        }

        [[nodiscard]] inline std::optional<FuncSym> get_func(const std::string& id) const {
            auto it = _funcs.find(id);
            return it == _funcs.end() ? std::nullopt : std::make_optional(it->second);
        }

        inline void var_init(const std::string& id) {
            for (auto it = _local.crbegin(); it != _local.crend(); ++it) {
                if (it->get_id() == id) {
                    const_cast<VarSym&>(*it).init();
                    return;
                }
            }

            if (auto it = _globs.find(id); it != _globs.end())
                it->second.init();
        }

        [[nodiscard]] inline auto& get_func_params(const std::string& id) const {
            return _funcs.find(id)->second.get_params();
        }

        [[nodiscard]] inline uint32_t get_cons_offset(Type type, const std::any& value) const {
            return _cons.find({ type, value })->second;
        }

        inline void put_cons(Type type, const std::any& value) {
            if (_cons.find({ type, value}) == _cons.end())
                _cons[{ type, value }] = _cons.size();
        }

        inline void put_glob(const std::string& id, Type type, uint32_t offset, uint32_t level,
                bool init = false, bool f_const = false) {
            _globs.insert({ id, VarSym(id, type, init, f_const, offset, level) });
        }

        inline void put_func(const std::string& id, Type ret, uint32_t offset) {
            _funcs.insert({ id, FuncSym(_funcs.size(), id, ret, offset) });
        }

        [[nodiscard]] inline bool put_param(const std::string& func, const std::string& param,
                Type type, bool f_const = false) {
            if (auto it = _funcs.find(func); it != _funcs.end())
                return it->second.put_param(param, type, f_const);
            return false;
        }

        inline void put_local(const std::string& id, Type type, uint32_t offset, uint32_t level,
                bool init = false, bool f_const = false) {
            _local.emplace_back(id, type, init, f_const, offset, level);
        }

        // when level a block
        inline void destroy_level(uint32_t level) {
            auto cnt = 0;
            for (auto it = _local.crbegin(); it != _local.crend() && it->get_level() == level; ++it)
                ++cnt;
            // why stl not support erase on reserve iterator :(
            _local.erase(_local.end() - cnt, _local.end());
        }

        // when a function end
        inline void clear_local() { _local.clear(); }
    };
}

namespace cc0 {
    using SymTbl = symbol::SymTbl;
}

#endif //C0_SYMTBL_H
