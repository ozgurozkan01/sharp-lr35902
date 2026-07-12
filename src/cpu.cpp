#include "../include/cpu.h"
#include <iomanip>

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
    

#if DEBUG_MOD
    print_debug();
#endif

    update_clock_cycles(cycle);

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
        case 0x14:
            inc(de.bytes.high);
            break;
        case 0x1c:
            inc(de.bytes.low);
            break;
        case 0x20:
            jr_cc(!get_flag(Flag::zero));
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
    std::cout  
          << "PC: 0x" << std::setfill('0') << std::setw(4) << std::hex << pc 
          << " | SP: 0x" << std::setfill('0') << std::setw(4) << sp 
          << " | Opcode: 0x" << std::setfill('0') << std::setw(2) << static_cast<int>(mmu.read(pc))
          << " | A: 0x" << std::setfill('0') << std::setw(2) << static_cast<int>(af.bytes.high) 
          << " | BC: 0x" << std::setfill('0') << std::setw(4) << bc.word
          << " | DE: 0x" << std::setfill('0') << std::setw(4) << de.word
          << " | HL: 0x" << std::setfill('0') << std::setw(4) << hl.word
          << " | Cycles: " << std::dec << total_cycle
          << " - Flags: ["
          << ((get_flag(Flag::zero))        ? 'Z' : '-')
          << ((get_flag(Flag::subtraction)) ? 'N' : '-')
          << ((get_flag(Flag::half_carry))  ? 'H' : '-')
          << ((get_flag(Flag::carry))       ? 'C' : '-')
          << "]" << std::endl; 
}

void CPU::update_clock_cycles(uint8_t cycle) noexcept {
    total_cycle += cycle;
}

void CPU::jp() noexcept {
    uint8_t low = mmu.read(pc + 1);
    uint8_t high = mmu.read(pc + 2);
    pc = (high << 8) | low;
}

void CPU::jr_cc(bool condition) noexcept {

    if (condition)
    {
        uint8_t unsigned_offset = mmu.read(pc + 1);
        int8_t signed_offset = static_cast<int8_t>(unsigned_offset);
        pc += signed_offset;
        
        update_clock_cycles(4);
    }
    
    pc += 2;
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
    uint8_t old_value = reg8;
    reg8++;
    pc++;

    reg8 == 0 ? set_flag(Flag::zero) : reset_flag(Flag::zero);
    reset_flag(Flag::subtraction);
    (old_value & 0x0f) == 0x0f ? set_flag(Flag::half_carry) : reset_flag(Flag::half_carry);
}

void CPU::inc(uint16_t &reg16) noexcept {
    reg16++;
    pc++;


}