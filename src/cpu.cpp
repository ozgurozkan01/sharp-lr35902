#include "../include/cpu.h"

#define DEBUG_MOD true

CPU::CPU(MMU &mmu) noexcept :
    mmu(mmu)
{
    af.word = 0x01B0;
    bc.word = 0x0013;
    de.word = 0x00d8;
    hl.word = 0x014d;
    pc = 0x100;
    sp = 0xfffe;

    total_cycle = 0;
}

void CPU::execute_instructions() noexcept {
    
    uint8_t opcode = mmu.read(pc);
    uint8_t cycle = cpu_clock_cycles[opcode];
    
    update_clock_cycles(cycle);

#if DEBUG_MOD
    print_debug();
#endif
    switch (opcode)
    {
        case 0x0:
            pc++;
            break;
        case 0x01:
            ld(bc.word);
            break;
        case 0x0E:
        {
            uint8_t data = mmu.read(pc + 1);
            ld(bc.bytes.low, data);
            pc++;
            break;
        }
        case 0x11:
            ld(de.word);
            break;
        case 0x12:
            ld(de.word, af.bytes.high);
            break;
        case 0x1c:

            break;
        case 0x21:
            ld(hl.word);
            break;
        case 0x2a:
        {
            uint8_t data = mmu.read(hl.word);
            ld(af.bytes.high, data);
            hl.word++;
            break;
        }
        case 0x31:
            ld(sp);
            break;
        case 0x47:
            ld(bc.bytes.high, af.bytes.high);
            break;
        case 0xc3:
            jp();
            break;
        default:
            std::cout << "DEFAULT CASE RAN : 0x" << std::hex << (int)opcode << std::endl;
            exit(-1);
    }
}

void CPU::set_flag(Flag flag) noexcept { af.bytes.low |= static_cast<uint8_t>(flag); }
void CPU::reset_flag(Flag flag) noexcept { af.bytes.low &= ~static_cast<uint8_t>(flag); }
bool CPU::get_flag(Flag flag) const noexcept { return (af.bytes.low & static_cast<uint8_t>(flag)) != 0; }

void CPU::print_debug() noexcept {

}

void CPU::update_clock_cycles(uint8_t cycle) noexcept {
    total_cycle += cycle;
}

void CPU::jp() {
    uint8_t low = mmu.read(pc + 1);
    uint8_t high = mmu.read(pc + 2);
    pc = (high << 8) | low;
}

void CPU::ld(uint16_t &reg16) noexcept {
    uint8_t low = mmu.read(pc + 1);
    uint8_t high = mmu.read(pc + 2);
    reg16 = (high << 8) | low;

    pc += 3;
}

void CPU::ld(uint8_t &target, uint8_t value) noexcept {
    target = value;
    pc++;
}

void CPU::ld(uint16_t address, uint8_t value) noexcept {
    mmu.write(address, value);
    pc++;
}

void CPU::inc(uint8_t &reg8) noexcept {
    
}

void CPU::inc(uint16_t &reg16) noexcept {

}