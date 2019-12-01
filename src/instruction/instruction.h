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
    class Instruction final {
        friend inline void swap(Instruction& lhs, Instruction& rhs) {
            std::swap(lhs._op_type, rhs._op_type);
            std::swap(lhs._op_code, rhs._op_code);
            std::swap(lhs._op_str, rhs._op_str);
            std::swap(lhs._op1, rhs._op1);
            std::swap(lhs._op2, rhs._op2);
        }

    private:
        InstructionType _op_type;
        uint8_t _op_code;
        std::string _op_str;

        std::optional<int32_t> _op1;
        std::optional<int32_t> _op2;

    public:
        // for two operands
        explicit Instruction(InstructionType type, int32_t op1, int32_t op2): _op_type(type), _op1(op1), _op2(op2) {
            auto [op_str, op_code] = utils::make_instruction(type);
            _op_str = std::move(op_str);
            _op_code = op_code;
        }
        // for one operand
        explicit Instruction(InstructionType type, int32_t op1): _op_type(type), _op1(op1), _op2(std::nullopt)  {
            auto [op_str, op_code] = utils::make_instruction(type);
            _op_str = std::move(op_str);
            _op_code = op_code;
        }
        // for no operand
        explicit Instruction(InstructionType type): _op_type(type), _op1(std::nullopt), _op2(std::nullopt) {
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
