/*
 * Created by zx5 on 2019/12/1.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_SYMTBL_H
#define C0_SYMTBL_H

#include "symbol.h"

#include <unordered_set>

namespace cc0 {
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

        [[nodiscard]] auto _find_local(const std::string& id, uint32_t domain) const;

    public:
        SymTbl() = default;
        SymTbl(const SymTbl&) = delete;
        SymTbl(SymTbl&&) = delete;
        SymTbl& operator=(SymTbl tbl) = delete;

        // when we use an identifier
        [[nodiscard]] inline bool is_declared(const std::string& id, uint32_t domain) const;
        // when we declare an identifier
        [[nodiscard]] inline bool is_declared(const std::string& id, uint32_t domain, uint32_t level) const;
        [[nodiscard]] inline bool is_const(const std::string& id, uint32_t domain) const;
        [[nodiscard]] inline bool is_init(const std::string& id, uint32_t domain) const;

        inline void init(const std::string& id, uint32_t domain);

        [[nodiscard]] inline Type get_type(const std::string& id, uint32_t domain) const;
        [[nodiscard]] inline auto& get_func_params(const std::string& id) const;
        [[nodiscard]] inline uint32_t get_cons_offset(Type type, const std::any& value) const;
        [[nodiscard]] inline uint32_t get_var_offset(const std::string& id, uint32_t domain) const;

        inline void put_cons(Type type, const std::any& value);
        inline void put_glob(const std::string& id, Type type, uint32_t offset, uint32_t domain, uint32_t level,
                bool init = false, bool f_const = false);
        inline void put_func(const std::string& id, Type ret, uint32_t offset);
        [[nodiscard]] inline bool put_param(const std::string& func, const std::string& param,
                Type type, bool f_const = false);
        inline void put_local(const std::string& id, Type type, uint32_t offset, uint32_t domain, uint32_t level,
                bool init = false, bool f_const = false);

        // when level a block
        inline void destroy_level(uint32_t domain, uint32_t level);
        // when return from a function
        inline void destroy_domain(uint32_t domain);
    };
}

#endif //C0_SYMTBL_H
