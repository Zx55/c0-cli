/*
 * Created by zx5 on 2019/12/2.
 * Copyright (c) zx5. All rights reserved.
 */

#include "symtbl.h"

namespace cc0 {
    // FIXME: never consider level
    inline bool SymTbl::is_declared(const std::string& id) const {
        return _funcs.find(id) != _funcs.end() || _vars.find(id) != _vars.end();
    }

    inline bool SymTbl::is_declared(const std::string& id, int level) const {

    }

    inline Type SymTbl::get_type(const std::string& id) const {
        return _vars.find(id)->second.get_type();
    }

    inline void SymTbl::assign(const std::string& id, std::any value) {
        // do type check and other check in semantic analyse, not here
        _vars.find(id)->second.set_value(std::move(value));
    }

    void SymTbl::leave() {

    }
}
 
 