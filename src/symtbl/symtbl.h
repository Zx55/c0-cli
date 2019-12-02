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
        struct _DomainLevel {
            std::string _id;
            uint32_t _domain;
            uint32_t _level;
        };

        std::unordered_set<ConstSym, ConstSym::_Hash> _consts;
        std::unordered_map<std::string, FuncSym> _funcs;

        /*
         * variable table
         * _globs - store global vars (_domain = 0)
         *          storing vars in two table is to make finding global vars easy.
         * _vars  - store local vars  (_domain > 0)
         */
        std::unordered_map<std::string, VarSym> _globs;
        std::vector<VarSym> _vars;

    public:
        SymTbl() = default;
        SymTbl(const SymTbl&) = delete;
        SymTbl(SymTbl&&) = delete;
        SymTbl& operator=(SymTbl tbl) = delete;
    };
}

#endif //C0_SYMTBL_H
