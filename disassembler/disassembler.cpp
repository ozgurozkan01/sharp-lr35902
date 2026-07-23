#include "disassembler.h"
#include "../include/mmu.h"
#include "../include/cpu.h"

#include <iomanip>

Disassembler::Disassembler(MMU &mmu, CPU &cpu) noexcept : mmu(mmu), cpu(cpu) {}

void Disassembler::decode() noexcept {
    uint16_t address = cpu.get_pc();
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
    
    print(decoded_instruction);
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

void Disassembler::print(const DecodedInstruction &instruction) noexcept {
    std::cout << std::format("{:#06x}  ", instruction.address);

    std::string bytesStr;
    for (uint8_t i = 0; i < instruction.byte_count; i++) {
        bytesStr += std::format("{:02X} ", instruction.raw_bytes[i]);
    }
    std::cout << std::format("{:<12}", bytesStr);

    if (instruction.cycles == instruction.cycles_branch) {
        std::cout << std::format("{:<6}", instruction.cycles);
    } else {
        std::cout << std::format("{:<6}", std::format("{}/{}", instruction.cycles, instruction.cycles_branch));
    }

    std::cout << std::format("{:<28}", instruction.disassembly);

    uint8_t f = cpu.get_f();
    char flagStr[5] = {
        (f & 0x80) ? 'Z' : '-',
        (f & 0x40) ? 'N' : '-',
        (f & 0x20) ? 'H' : '-',
        (f & 0x10) ? 'C' : '-',
        '\0'
    };

    std::cout << std::format(
        "PC:{:04X}   SP:{:04X}   A:{:02X}   F:{}   BC:{:04X}   DE:{:04X}   HL:{:04X}\n",
        cpu.get_pc(), cpu.get_sp(), cpu.get_a(), flagStr, cpu.get_bc(), cpu.get_de(), cpu.get_hl()
    );
}