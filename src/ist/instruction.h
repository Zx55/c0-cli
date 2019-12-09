/*
 * Created by zx5 on 2019/11/30.
 * Copyright (c) zx5. All rights reserved.
 */

#ifndef C0_INSTRUCTION_H
#define C0_INSTRUCTION_H

#include "tools/alias.h"
#include "tools/enums.h"
#include "tools/utils.h"

#include <string>
#include <optional>

namespace cc0 {
    namespace utils {
        struct _MakeIstRes final {
            std::string str;
            uint8_t code;
            uint8_t op_size1;
            uint8_t op_size2;
        };

        inline _MakeIstRes make_instruction(InstType type) {
            switch (type) {
                case InstType::NOP:     return { "nop",     0x00, 9, 9 };
                case InstType::BIPUSH:  return { "bipush",  0x01, 1, 9 };
                case InstType::IPUSH:   return { "ipush",   0x02, 4, 9 };
                case InstType::POP:     return { "pop",     0x04, 9, 9 };
                case InstType::POP2:    return { "pop2",    0x05, 9, 9 };
                case InstType::POPN:    return { "popn",    0x06, 4, 9 };
                case InstType::DUP:     return { "dup",     0x07, 9, 9 };
                case InstType::DUP2:    return { "dup2",    0x08, 9, 9 };
                case InstType::LOADC:   return { "loadc",   0x09, 2, 9 };
                case InstType::LOADA:   return { "loada",   0x0a, 2, 4 };
                case InstType::SNEW:    return { "snew",    0x0c, 4, 9 };
                case InstType::ILOAD:   return { "iload",   0x10, 9, 9 };
                case InstType::DLOAD:   return { "dload",   0x11, 9, 9 };
                case InstType::ISTORE:  return { "istore",  0x20, 9, 9 };
                case InstType::DSTORE:  return { "dstore",  0x21, 9, 9 };
                case InstType::IADD:    return { "iadd",    0x30, 9, 9 };
                case InstType::DADD:    return { "dadd",    0x31, 9, 9 };
                case InstType::ISUB:    return { "isub",    0x34, 9, 9 };
                case InstType::DSUB:    return { "dsub",    0x35, 9, 9 };
                case InstType::IMUL:    return { "imul",    0x38, 9, 9 };
                case InstType::DMUL:    return { "dmul",    0x39, 9, 9 };
                case InstType::IDIV:    return { "idiv",    0x3c, 9, 9 };
                case InstType::DDIV:    return { "ddiv",    0x3d, 9, 9 };
                case InstType::INEG:    return { "ineg",    0x40, 9, 9 };
                case InstType::DNEG:    return { "dneg",    0x41, 9, 9 };
                case InstType::ICMP:    return { "icmp",    0x44, 9, 9 };
                case InstType::DCMP:    return { "dcmp",    0x45, 9, 9 };
                case InstType::I2D:     return { "i2d",     0x60, 9, 9 };
                case InstType::D2I:     return { "d2i",     0x61, 9, 9 };
                case InstType::I2C:     return { "i2c",     0x62, 9, 9 };
                case InstType::JMP:     return { "jmp",     0x70, 2, 9 };
                case InstType::JE:      return { "je",      0x71, 2, 9 };
                case InstType::JNE:     return { "jne",     0x72, 2, 9 };
                case InstType::JL:      return { "jl",      0x73, 2, 9 };
                case InstType::JGE:     return { "jge",     0x74, 2, 9 };
                case InstType::JG:      return { "jg",      0x75, 2, 9 };
                case InstType::JLE:     return { "jle",     0x76, 2, 9 };
                case InstType::CALL:    return { "call",    0x80, 2, 9 };
                case InstType::RET:     return { "ret",     0x88, 9, 9 };
                case InstType::IRET:    return { "iret",    0x89, 9, 9 };
                case InstType::DRET:    return { "dret",    0x8a, 9, 9 };
                case InstType::IPRINT:  return { "iprint",  0xa0, 9, 9 };
                case InstType::DPRINT:  return { "dprint",  0xa1, 9, 9 };
                case InstType::CPRINT:  return { "cprint",  0xa2, 9, 9 };
                case InstType::SPRINT:  return { "sprint",  0xa3, 9, 9 };
                case InstType::PRINTL:  return { "printl",  0xaf, 9, 9 };
                case InstType::ISCAN:   return { "iscan",   0xb0, 9, 9 };
                case InstType::DSCAN:   return { "dscan",   0xb1, 9, 9 };
                case InstType::CSCAN:   return { "cscan",   0xb2, 9, 9 };
                default:                return { "nop",     0x00, 9, 9 };
            }
        }
    }

    class Instruction final {
        friend inline void swap(Instruction& lhs, Instruction& rhs) {
            std::swap(lhs._op_type, rhs._op_type);
            std::swap(lhs._op_str, rhs._op_str);
            std::swap(lhs._op1, rhs._op1);
            std::swap(lhs._op2, rhs._op2);
        }

        friend inline std::ostream& operator<<(std::ostream& out, const Instruction& rhs) {
            out << utils::make_instruction(rhs._op_type).str;
            if (rhs._op1.has_value())
                out << ' ' << rhs._op1.value();
            if (rhs._op2.has_value())
                out << ", " << rhs._op2.value();
            return out;
        }

    private:
        InstType _op_type;
        std::string _op_str;

        std::optional<uint32_t> _op1;
        std::optional<uint32_t> _op2;

    public:
        // for two operands
        explicit Instruction(InstType type, uint32_t op1, uint32_t op2): _op_type(type), _op1(op1), _op2(op2) {
            auto res = utils::make_instruction(type);
            _op_str = std::move(res.str);
        }
        // for one operand
        explicit Instruction(InstType type, uint32_t op1): _op_type(type), _op1(op1), _op2(std::nullopt)  {
            auto res = utils::make_instruction(type);
            _op_str = std::move(res.str);
        }
        // for no operand
        explicit Instruction(InstType type): _op_type(type), _op1(std::nullopt), _op2(std::nullopt) {
            auto res = utils::make_instruction(type);
            _op_str = std::move(res.str);
        }
        Instruction(const Instruction&) = default;
        Instruction(Instruction&&) = default;
        Instruction& operator=(Instruction instruction) {
            swap(*this, instruction);
            return *this;
        }
        bool operator==(const Instruction& rhs) const {
            return _op_type == rhs._op_type && _op1 == rhs._op1 && _op2 == rhs._op2;
        }

        [[nodiscard]] inline InstType get_type() const { return _op_type; }
        void set_op1(uint32_t op1) { _op1 = op1; }

        inline void output_bin(std::ostream& out) const {
            auto res = utils::make_instruction(_op_type);

            out.write(reinterpret_cast<char*>(&res.code), 1);

            if (res.op_size1 <= 4) {
                switch (res.op_size1) {
                    case 1: {
                        auto op1 = static_cast<uint8_t>(_op1.value());
                        out.write(reinterpret_cast<char*>(&op1), 1);
                        break;
                    }
                    case 2: {
                        uint16_t op1 = utils::swap_endian(static_cast<uint16_t>(_op1.value()));
                        out.write(reinterpret_cast<char*>(&op1), 2);
                        break;
                    }
                    case 4: {
                        auto op1 = utils::swap_endian(static_cast<uint32_t>(_op1.value()));
                        out.write(reinterpret_cast<char*>(&op1), 4);
                        break;
                    }
                }
            }

            if (res.op_size2 <= 4) {
                switch (res.op_size2) {
                    case 1: {
                        auto op2 = static_cast<uint8_t>(_op2.value());
                        out.write(reinterpret_cast<char*>(&op2), 1);
                        break;
                    }
                    case 2: {
                        uint16_t op2 = utils::swap_endian(static_cast<uint16_t>(_op2.value()));
                        out.write(reinterpret_cast<char*>(&op2), 2);
                        break;
                    }
                    case 4: {
                        auto op2 = utils::swap_endian(static_cast<uint32_t>(_op2.value()));
                        out.write(reinterpret_cast<char*>(&op2), 4);
                        break;
                    }
                }
            }
        }
    };
}

#endif //C0_INSTRUCTION_H
