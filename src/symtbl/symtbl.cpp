/*
 * Created by zx5 on 2019/12/2.
 * Copyright (c) zx5. All rights reserved.
 */

#include "symtbl.h"

namespace cc0::symbol {
    std::optional<VarSym> SymTbl::get_var(const std::string& id) const {
        for (auto it = _local.crbegin(); it != _local.crend(); ++it)
            if (it->get_id() == id) return *it;
        if (auto it = _globs.find(id); it != _globs.end())
            return it->second;
        return std::nullopt;
    }

    std::optional<VarSym> SymTbl::get_var(const std::string& id, cc0::uint32_t level) const {
        for (auto it = _local.crbegin(); it != _local.crend() && it->get_level() == level; ++it)
            if (it->get_id() == id) return *it;
        return std::nullopt;
    }

    void SymTbl::init(const std::string& id) {
        for (auto it = _local.crbegin(); it != _local.crend(); ++it) {
            if (it->get_id() == id) {
                const_cast<VarSym&>(*it).init();
                return;
            }
        }

        if (auto it = _globs.find(id); it != _globs.end())
            it->second.init();
    }

    auto& SymTbl::get_func_params(const std::string& id) const {
        return _funcs.find(id)->second.get_params();
    }

    uint32_t SymTbl::get_cons_offset(const Type type, const std::any& value) const {
        return _cons.find({ type, value })->second;
    }

    void SymTbl::put_cons(Type type, const std::any& value) {
        if (_cons.find({ type, value}) == _cons.end())
            _cons[{ type, value }] = _cons.size();
    }

    void SymTbl::put_glob(const std::string& id, Type type, uint32_t offset,
            uint32_t level, bool init, bool f_const) {
        _globs.insert({ id, VarSym(id, type, init, f_const, offset, level) });
    }

    void SymTbl::put_func(const std::string& id, Type ret, uint32_t offset) {
        _funcs.insert({ id, FuncSym(_funcs.size(), id, ret, offset) });
    }

    bool SymTbl::put_param(const std::string& func, const std::string& param, Type type, bool f_const) {
        if (auto it = _funcs.find(func); it != _funcs.end())
            return it->second.put_param(param, type, f_const);
    }

    void SymTbl::put_local(const std::string& id, Type type, uint32_t offset, uint32_t level,
            bool init, bool f_const) {
        _local.emplace_back(id, type, init, f_const, offset, level);
    }

    void SymTbl::destroy_level(uint32_t level) {
        auto cnt = 0;
        for (auto it = _local.crbegin(); it != _local.crend() && it->get_level() == level; ++it)
                ++cnt;
        // why stl not support erase on reserve iterator :(
        _local.erase(_local.end() - cnt, _local.end());
    }
}
 
 