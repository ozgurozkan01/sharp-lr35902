#include "disassembler.h"
#include "../include/mmu.h"

#include <iomanip>

Disassembler::Disassembler(MMU &mmu) noexcept : mmu(mmu) {}

DecodedInstruction Disassembler::decode(uint16_t address) noexcept {
    DecodedInstruction decoded_instruction;
    decoded_instruction.address = address;

    const OpcodeInfo* info = nullptr;
    uint8_t opcode = mmu.read(address);
    uint16_t operand_start_address = address + 1;

    if (opcode == 0xCB) {
        uint8_t prefixed_opcode = mmu.read(address + 1);
        info = &prefixed_instruction_set[prefixed_opcode];

        operand_start_address = address + 2;

        decoded_instruction.is_cb_prefixed = true;

        decoded_instruction.raw_bytes[0] = opcode;
        decoded_instruction.raw_bytes[1] = prefixed_opcode;
    }
    else {
        info = &standard_instruction_set[opcode];
        decoded_instruction.raw_bytes[0] = opcode;
    }

    decoded_instruction.byte_count = info->length + (decoded_instruction.is_cb_prefixed ? 1 : 0);
    decoded_instruction.cycles = info->cycle;
    decoded_instruction.cycles_branch = info->cycles_branch;

    uint8_t extra_bytes = info->length - 1;
    for (uint8_t i = 0; i < extra_bytes; i++) {
        uint8_t index = decoded_instruction.is_cb_prefixed ? (2 + i) : (1 + i);
        decoded_instruction.raw_bytes[index] = mmu.read(address + index);
    }

    decoded_instruction.disassembly = format_text(*info, address, operand_start_address, extra_bytes);

    return decoded_instruction;
}

std::string Disassembler::format_text(const OpcodeInfo& info, uint16_t address, uint16_t operand_address, uint8_t extra_bytes) const noexcept {
    if (extra_bytes == 0) // NOP, RLCA ...
    {
        return std::string(info.mnemonic);
    }
    
    if (info.is_relative) // JR ...
    {
        int8_t signed_offset = static_cast<int8_t>(mmu.read(operand_address));
        uint16_t target = address + info.length + signed_offset;
        return std::vformat(info.mnemonic, std::make_format_args(target));
    }
    
    if (extra_bytes == 1) // Immediate data operations
    {
        uint8_t immediate8 = mmu.read(operand_address);
        return std::vformat(info.mnemonic, std::make_format_args(immediate8));
    }

    // extra bytes == 2
    uint16_t immediate16 = mmu.read(operand_address) | (mmu.read(operand_address + 1) << 8);
    return std::vformat(info.mnemonic, std::make_format_args(immediate16));
}

