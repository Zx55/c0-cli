/*
 * Created by zx5 on 2019/12/4.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_GEN_CTX_H
#define C0_GEN_CTX_H

#include "tools/error.h"
#include "symtbl/symtbl.h"
#include "ist/instruction.h"

namespace cc0 {
    class GeneratorContext final {
    private:
        static SymTbl _tbl;
        static std::vector<C0Err> _fatal;
        static std::vector<C0Err> _wrns;
        static std::vector<Instruction> _ist;

    public:
        GeneratorContext() = delete;
        GeneratorContext(const GeneratorContext&) = delete;
        GeneratorContext(GeneratorContext&&) = delete;
        GeneratorContext& operator=(GeneratorContext ctx) = delete;

        inline static void init_tbl() { _tbl.init(); }
        inline static SymTbl& get_tbl() { return _tbl; }
        [[nodiscard]] inline static auto&& get_fatals() { return std::move(_fatal); }
        [[nodiscard]] inline static auto&& get_wrns() { return std::move(_wrns); }
        [[nodiscard]] inline static auto&& get_ist() { return std::move(_ist); }

        inline static void put_fatal(C0Err err) { _fatal.push_back(std::move(err)); }
        inline static void put_wrn(C0Err wrn) { _wrns.push_back(std::move(wrn)); }
        inline static void put_ist(Instruction ist) { _ist.push_back(std::move(ist)); }

        [[nodiscard]] inline static Instruction& get_ist(uint32_t offset) { return _ist.at(offset); }
        inline static void pop_ist() { _ist.pop_back(); }
        inline static void pop_ist(uint32_t n) { _ist.erase(_ist.end() - n, _ist.end()); }
    };
}

#endif //C0_GEN_CTX_H
