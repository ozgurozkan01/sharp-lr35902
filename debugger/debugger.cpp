#include "debugger.h"
#include "disassembler.h"
#include "../include/cpu.h"
#include "../include/mmu.h"
#include "../include/timer.h"
#include "../include/interrupt_controller.h"

#include <iostream>
#include <functional>
#include <format>

Debugger::Debugger(CPU &cpu, MMU &mmu, Timer &timer, InterruptController &interrupt_controller, Disassembler &disassembler) noexcept :
    cpu(cpu),
    mmu(mmu),
    timer(timer),
    interrupt_controller(interrupt_controller),
    disassembler(disassembler)
{}

void Debugger::print() noexcept {
    uint16_t address = cpu.get_pc();
    DecodedInstruction instruction = disassembler.decode(address);

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
