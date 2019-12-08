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

#define MAGIC 0x43303A29
#define VERSION 0x00000001

#define _bswap(_byte) utils::swap_endian(_byte)
#define _gen_cast(_value) reinterpret_cast<char*>(&(_value))

namespace cc0 {
    class Generator {
    private:
        ast::_ptr<ast::RootAST>& _root;

        [[nodiscard]] inline static std::string _make_index(uint32_t index) {
            return fmt::format("{:<5d} ", index);
        }

        inline static void write_func_info(std::ostream& out, uint16_t name_offset,
                uint16_t slot, uint16_t length) {
            uint16_t level = 1;

            name_offset = _bswap(name_offset);
            slot = _bswap(slot);
            level = _bswap(level);
            length = _bswap(length);

            out.write(_gen_cast(name_offset), 2);
            out.write(_gen_cast(slot), 2);
            out.write(_gen_cast(level), 2);
            out.write(_gen_cast(length), 2);
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
            const auto& funcs = tbl.get_func_tbl();
            const auto& ist = RuntimeContext::get_ist();
            uint32_t index = 0;

            out << ".constants:\n";
            for (const auto& cons: tbl.get_cons_tbl())
                out << _make_index(index++) << cons << '\n';

            out << ".start:\n";
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
            for (index = 0, start = end, end = ist.size(); start != end; ++start)
                out << _make_index(index++) << ist[start] << '\n';
        }

        static inline void output_bin(std::ostream& out) {
            const auto& tbl = GeneratorContext::get_tbl();
            const auto& funcs = tbl.get_func_tbl();
            const auto& ist = RuntimeContext::get_ist();

            // magic
            uint32_t magic = _bswap(MAGIC);
            out.write(_gen_cast(magic), 4);
            uint32_t version = _bswap(VERSION);
            out.write(_gen_cast(version), 4);

            // constant
            const auto& cons_tbl = tbl.get_cons_tbl();
            uint16_t cons_num = _bswap(static_cast<uint16_t>(cons_tbl.size()));
            out.write(_gen_cast(cons_num), 2);
            for (const auto& cons: cons_tbl)
                cons.output_bin(out);

            // start code
            uint16_t offset = funcs.empty() ? ist.size() : funcs.at(0)->second.get_offset();
            uint16_t reversed_offset = _bswap(offset);
            out.write(_gen_cast(reversed_offset), 2);
            for (auto i = 0u; i != offset; ++i)
                ist[i].output_bin(out);

            // functions
            uint16_t func_num = funcs.size();
            func_num = _bswap(func_num);
            out.write(_gen_cast(func_num), 2);

            if (funcs.empty()) return;
            if (funcs.size() == 1) {
                write_func_info(out, tbl.get_cons_offset(Type::STRING, *funcs.begin()),
                        funcs.at(0)->second.get_slot(), ist.size() - offset);
                for (auto i = offset; i != ist.size(); ++i)
                    ist[i].output_bin(out);
                return;
            }

            uint32_t p, start, end;
            for (p = 1, end = offset; p != funcs.size(); ++p) {
                start = end, end = funcs.at(p)->second.get_offset();
                write_func_info(out, tbl.get_cons_offset(Type::STRING, *(funcs.begin() + p - 1)),
                        funcs.at(p - 1)->second.get_slot(), end - start);
                for (; start != end; ++start)
                    ist[start].output_bin(out);
            }

            start = end, end = ist.size();
            write_func_info(out, tbl.get_cons_offset(Type::STRING, *(funcs.end() - 1)),
                    funcs.at(p - 1)->second.get_slot(), end - start);
            for (; start != end; ++start)
                ist[start].output_bin(out);
        }
    };
}

#endif //C0_GENERATOR_H
