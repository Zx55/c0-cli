/*
 * Created by zx5 on 2019/12/2.
 * Copyright (c) zx5. All rights reserved.
 */

#include "symtbl.h"

#include <utility>

namespace cc0 {
    auto SymTbl::_find_local(const std::string& id, const uint32_t domain) const {
        for (auto it = _local.crbegin(); it != _local.crend() && it->get_scope().first == domain; ++it) {
            if (it->get_id() == id)
                return it;
        }
        return _local.crend();
    }

    bool SymTbl::is_declared(const std::string& id, const uint32_t domain) const {
        if (_globs.find(id) != _globs.end())
            return true;
        if (_funcs.find(id) != _funcs.end())
            return true;
        if (_find_local(id, domain) != _local.crend())
            return true;
        return false;
    }

    bool SymTbl::is_declared(const std::string& id, uint32_t domain, uint32_t level) const {
        for (auto it = _local.crbegin(); it != _local.crend() && it->get_scope().first == domain
            && it->get_scope().second == level; ++it) {
            if (it->get_id() == id) return true;
        }
        return false;
    }

    bool SymTbl::is_const(const std::string& id, const uint32_t domain) const {
        if (auto it = _globs.find(id); it != _globs.end())
            return it->second.is_const();
        if (auto it = _find_local(id, domain); it != _local.crend())
            return it->is_const();
        return false;
    }

    bool SymTbl::is_init(const std::string& id, const uint32_t domain) const {
        if (auto it = _globs.find(id); it != _globs.end())
            return it->second.is_init();
        if (auto it = _find_local(id, domain); it != _local.crend())
            return it->is_init();
        return false;
    }

    void SymTbl::init(const std::string& id, const uint32_t domain) {
        if (auto it = _globs.find(id); it != _globs.end()) {
            it->second.init();
            return;
        }
        if (auto it = _find_local(id, domain); it != _local.crend())
            const_cast<VarSym&>(*it).init();
    }

    Type SymTbl::get_type(const std::string& id, const uint32_t domain) const {
        if (auto it = _globs.find(id); it != _globs.end())
            return it->second.get_type();
        if (auto it = _find_local(id, domain); it != _local.crend())
            return it->get_type();
        return Type::UNDEFINED;
    }

    auto& SymTbl::get_func_params(const std::string& id) const {
        return _funcs.find(id)->second.get_params();
    }

    uint32_t SymTbl::get_cons_offset(const Type type, const std::any& value) const {
        return _cons.find({ type, value })->second;
    }

    uint32_t SymTbl::get_var_offset(const std::string& id, const uint32_t domain) const {
        if (auto it = _globs.find(id); it != _globs.end())
            return it->second.get_offset();
        if (auto it = _find_local(id, domain); it != _local.crend())
            return it->get_offset();
        return -1;
    }

    void SymTbl::put_cons(Type type, const std::any& value) {
        if (_cons.find({ type, value}) == _cons.end())
            _cons[{ type, value }] = _cons.size();
    }

    void SymTbl::put_glob(const std::string& id, Type type, uint32_t offset, uint32_t domain,
            uint32_t level, bool init, bool f_const) {
        _globs.insert({ id, VarSym(id, type, init, f_const, offset, domain, level) });
    }

    void SymTbl::put_func(const std::string& id, Type ret, uint32_t offset) {
        _funcs.insert({ id, FuncSym(_funcs.size(), id, ret, offset) });
    }

    bool SymTbl::put_param(const std::string& func, const std::string& param, Type type, bool f_const) {
        if (auto it = _funcs.find(func); it != _funcs.end())
            return it->second.put_param(param, type, f_const);
    }

    void SymTbl::put_local(const std::string& id, Type type, uint32_t offset, uint32_t domain, uint32_t level,
            bool init, bool f_const) {
        _local.emplace_back(id, type, init, f_const, offset, domain, level);
    }

    void SymTbl::destroy_level(uint32_t domain, uint32_t level) {
        auto cnt = 0;
        for (auto it = _local.crbegin(); it != _local.crend() && it->get_scope().first == domain
            && it->get_scope().second == level; ++it)
                ++cnt;
        // why stl don't support erase on reserve iterator :(
        _local.erase(_local.end() - cnt, _local.end());
    }

    void SymTbl::destroy_domain(uint32_t domain) {
        auto cnt = 0;
        for (auto it = _local.crbegin(); it != _local.crend() && it->get_scope().first == domain; ++it)
            ++cnt;
        _local.erase(_local.end() - cnt, _local.end());
    }
}
 
 