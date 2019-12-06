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

#define _gen_func(_idx) tbl.get_func(funcs[_idx])

namespace cc0 {
    class Generator {
    private:
        ast::_ptr<ast::RootAST>& _root;

        [[nodiscard]] inline static std::string _make_index(uint32_t index) {
            return fmt::format("{:<5d} ", index);
        }

        [[nodiscard]] inline static uint32_t _make_slot(Type type) {
            switch (type) {
                case Type::INT:     [[fallthrough]];
                case Type::CHAR:    return 1;
                case Type::DOUBLE:  return 2;
                default:            return -1;
            }
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

        inline void output_ist(std::ostream& out) const {
            const auto& tbl = GeneratorContext::get_tbl();
            // get function name
            const auto funcs = _root->get_funcs();
            auto ist = RuntimeContext::get_ist();

            out << ".constants:\n";
            for (const auto& cons: tbl.get_cons_tbl())
                out << cons << '\n';

            out << ".start:\n";
            // make sure there is no error
            auto offset = tbl.get_func(funcs.front())->get_offset();
            for (auto i = 0u; i != offset; ++i)
                out << _make_index(i) << RuntimeContext::get_ist()[i] << '\n';

            out << ".functions:\n";
            uint32_t index = 0;
            for (const auto& func: funcs) {
                out << _make_index(index++) << tbl.get_cons_offset(Type::STRING, func) << ' ';

                // TODO: calculate slot in ast
                uint32_t slot = 0;
                const auto params = tbl.get_func(func)->get_params();
                for (const auto& param: params) {
                    const auto& [type, f_const] = param.second;
                    slot += _make_slot(type);
                }

                out << slot << " 1\n";
            }

            if (funcs.empty()) return;
            if (funcs.size() == 1) {
                index = 0;

                out << ".F0:\n";
                for (auto i = offset; i != RuntimeContext::get_ist().size(); ++i)
                    out << _make_index(index++) << RuntimeContext::get_ist()[i] << '\n';

                return;
            }

            // TODO multifunction
            return;
        }
    };
}

#endif //C0_GENERATOR_H
