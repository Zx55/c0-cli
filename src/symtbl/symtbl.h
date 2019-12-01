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
        std::unordered_set<ConstSym, ConstSym::_Hash> _consts;
        std::unordered_map<std::string, FuncSym> _funcs;
        std::unordered_map<std::string, VarSym> _vars;
        std::vector<std::pair<std::string, uint32_t>> _level_stk;

    public:
        SymTbl() = default;
        SymTbl(const SymTbl&) = delete;
        SymTbl(SymTbl&&) = delete;
        SymTbl& operator=(SymTbl tbl) = delete;

        [[nodiscard]] inline bool is_declared(const std::string& id) const;
        [[nodiscard]] inline bool is_declared(const std::string& id, int level) const;

        [[nodiscard]] inline Type get_type(const std::string& id) const;

        inline void assign(const std::string& id, std::any value);
        void leave();   // leave a block, destroy vars in _level_stk
    };
}

#endif //C0_SYMTBL_H
