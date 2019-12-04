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
        inline std::pair<std::string, uint8_t> make_instruction(InstType type) {
            switch (type) {
                case InstType::NOP:     return { "nop",     0x00 };
                case InstType::BIPUSH:  return { "bipush",  0x01 };
                case InstType::IPUSH:   return { "ipush",   0x02 };
                case InstType::POP:     return { "pop",     0x04 };
                case InstType::POP2:    return { "pop2",    0x05 };
                case InstType::POPN:    return { "popn",    0x06 };
                case InstType::DUP:     return { "dup",     0x07 };
                case InstType::DUP2:    return { "dup2",    0x08 };
                case InstType::LOADC:   return { "loadc",   0x09 };
                case InstType::LOADA:   return { "loada",   0x0a };
                case InstType::NEW:     return { "new",     0x0b };
                case InstType::SNEW:    return { "snew",    0x0c };
                case InstType::ILOAD:   return { "iload",   0x10 };
                case InstType::DLOAD:   return { "dload",   0x11 };
                case InstType::ALOAD:   return { "aload",   0x12 };
                case InstType::ISTORE:  return { "istore",  0x20 };
                case InstType::DSTORE:  return { "dstore",  0x21 };
                case InstType::ASTORE:  return { "astore",  0x22 };
                case InstType::IADD:    return { "iadd",    0x30 };
                case InstType::DADD:    return { "dadd",    0x31 };
                case InstType::ISUB:    return { "isub",    0x34 };
                case InstType::DSUB:    return { "dsub",    0x35 };
                case InstType::IMUL:    return { "imul",    0x38 };
                case InstType::DMUL:    return { "dmul",    0x39 };
                case InstType::IDIV:    return { "idiv",    0x3c };
                case InstType::DDIV:    return { "ddiv",    0x3d };
                case InstType::INEG:    return { "ineg",    0x40 };
                case InstType::DNEG:    return { "dneg",    0x41 };
                case InstType::ICMP:    return { "icmp",    0x44 };
                case InstType::DCMP:    return { "dcmp",    0x45 };
                case InstType::I2D:     return { "i2d",     0x60 };
                case InstType::D2I:     return { "d2i",     0x61 };
                case InstType::I2C:     return { "i2c",     0x62 };
                case InstType::JMP:     return { "jmp",     0x70 };
                case InstType::JE:      return { "je",      0x71 };
                case InstType::JNE:     return { "jne",     0x72 };
                case InstType::JL:      return { "jl",      0x73 };
                case InstType::JGE:     return { "jge",     0x74 };
                case InstType::JG:      return { "jg",      0x75 };
                case InstType::JLE:     return { "jle",     0x76 };
                case InstType::CALL:    return { "call",    0x80 };
                case InstType::RET:     return { "ret",     0x88 };
                case InstType::IRET:    return { "iret",    0x89 };
                case InstType::DRET:    return { "dret",    0x8a };
                case InstType::ARET:    return { "aret",    0x8b };
                case InstType::IPRINT:  return { "iprint",  0xa0 };
                case InstType::DPRINT:  return { "dprint",  0xa1 };
                case InstType::CPRINT:  return { "cprint",  0xa2 };
                case InstType::SPRINT:  return { "sprint",  0xa3 };
                case InstType::PRINTL:  return { "printl",  0xaf };
                case InstType::ISCAN:   return { "iscan",   0xb0 };
                case InstType::DSCAN:   return { "dscan",   0xb1 };
                case InstType::CSCAN:   return { "cscan",   0xb2 };
                default:                return { "nop",     0x00 };
            }
        }
    }

    class Instruction final {
        friend inline void swap(Instruction& lhs, Instruction& rhs) {
            std::swap(lhs._op_type, rhs._op_type);
            std::swap(lhs._op_code, rhs._op_code);
            std::swap(lhs._op_str, rhs._op_str);
            std::swap(lhs._op1, rhs._op1);
            std::swap(lhs._op2, rhs._op2);
        }

    private:
        InstType _op_type;
        uint8_t _op_code;
        std::string _op_str;

        std::optional<int32_t> _op1;
        std::optional<int32_t> _op2;

    public:
        // for two operands
        explicit Instruction(InstType type, int32_t op1, int32_t op2): _op_type(type), _op1(op1), _op2(op2) {
            auto [op_str, op_code] = utils::make_instruction(type);
            _op_str = std::move(op_str);
            _op_code = op_code;
        }
        // for one operand
        explicit Instruction(InstType type, int32_t op1): _op_type(type), _op1(op1), _op2(std::nullopt)  {
            auto [op_str, op_code] = utils::make_instruction(type);
            _op_str = std::move(op_str);
            _op_code = op_code;
        }
        // for no operand
        explicit Instruction(InstType type): _op_type(type), _op1(std::nullopt), _op2(std::nullopt) {
            auto [op_str, op_code] = utils::make_instruction(type);
            _op_str = std::move(op_str);
            _op_code = op_code;
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
    };
}

#endif //C0_INSTRUCTION_H
