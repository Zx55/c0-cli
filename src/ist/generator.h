/*
 * Created by zx5 on 2019/12/4.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_GENERATOR_H
#define C0_GENERATOR_H

#include "tools/alias.h"
#include "ctx/rt_ctx.h"
#include "ctx/gen_ctx.h"

#include "fmt/format.h"

namespace cc0 {
    class Generator {
    private:
        ast::_ptr<ast::RootAST>& _root;

        [[nodiscard]] inline static std::string _make_index(uint32_t index) {
            return fmt::format("{:<5d} ", index);
        }

    public:
        Generator(): _root(RuntimeContext::get_ast()) { }
        Generator(const Generator&) = delete;
        Generator(Generator&&) = delete;
        Generator& operator=(const Generator&) = delete;

        inline void generate() const {
            GeneratorContext::init_tbl();
            (void) _root->generate({ 0, 0, 0, Type::UNDEFINED });

            RuntimeContext::set_ist(std::move(GeneratorContext::get_ist()));
            RuntimeContext::put_fatals(std::move(GeneratorContext::get_fatals()));
            RuntimeContext::put_wrns(std::move(GeneratorContext::get_wrns()));
        }

        static inline void output_ist(std::ostream& out) {
            const auto& tbl = GeneratorContext::get_tbl();
            // get function name
            const auto& funcs = tbl.get_func_tbl();
            auto ist = RuntimeContext::get_ist();
            uint32_t index = 0;

            out << ".constants:\n";
            for (const auto& cons: tbl.get_cons_tbl())
                out << _make_index(index++) << cons << '\n';

            out << ".start:\n";
            // make sure there is no error
            auto offset = funcs.empty() ? ist.size() : funcs.at(0)->second.get_offset();
            for (auto i = 0u; i != offset; ++i)
                out << _make_index(i) << ist[i] << '\n';

            out << ".functions:\n";
            index = 0;
            for (const auto& func: funcs) {
                out << _make_index(index++) << tbl.get_cons_offset(Type::STRING, func) << ' ';
                out << tbl.get_func(func)->get_slot() << " 1\n";
            }

            if (funcs.empty()) return;
            if (funcs.size() == 1) {
                index = 0;

                out << ".F0:\n";
                for (auto i = offset; i != ist.size(); ++i)
                    out << _make_index(index++) << ist[i] << '\n';

                return;
            }

            uint32_t p, start, end;
            for (p = 1, end = offset; p != funcs.size(); ++p) {
                out << ".F" << (p - 1) << '\n';
                for (index = 0, start = end, end = funcs.at(p)->second.get_offset(); start != end; ++start)
                    out << _make_index(index++) << ist[start] << '\n';
            }

            out << ".F" << (p - 1) << '\n';
            for (index = 0, start = end; start != ist.size(); ++start)
                out << _make_index(index++) << ist[start] << '\n';
        }
    };
}

#endif //C0_GENERATOR_H
