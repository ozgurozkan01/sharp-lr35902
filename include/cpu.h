#ifndef CPU_H
#define CPU_H

#include "mmu.h"

#include <iostream>
#include <stdint.h>


typedef union {
    uint16_t word;
        
    struct {
        uint8_t low;
        uint8_t high;
    } bytes;

} Register_pair;


class CPU {
public:

    CPU(MMU &mmu);

    void execute_instructions();

private:
    
    enum class Flag : uint8_t {
        carry       = 0x10,
        half_carry  = 0x20,
        subtraction = 0x40,
        zero        = 0x80
    };

    void print_debug();
    void update_clock_cycles(uint8_t cycle);

    void inc(uint8_t &reg8);
    void inc(uint16_t &reg16);

    void jp();

    void ld(uint16_t &reg16);
    void ld(uint8_t &target, uint8_t value);
    void ld(uint16_t address, uint8_t value);

    Register_pair af;
    Register_pair bc;
    Register_pair de;
    Register_pair hl;

    uint16_t sp;
    uint16_t pc;

    MMU &mmu;
    
    uint64_t total_cycle;
    const uint8_t cpu_clock_cycles[256] = {
    //  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
        4,  12, 8,  8,  4,  4,  8,  4,  20, 8,  8,  8,  4,  4,  8,  4,  // 0
        4,  12, 8,  8,  4,  4,  8,  4,  12, 8,  8,  8,  4,  4,  8,  4,  // 1
        8,  12, 8,  8,  4,  4,  8,  4,  8,  8,  8,  8,  4,  4,  8,  4,  // 2
        8,  12, 8,  8,  12, 12, 12, 4,  8,  8,  8,  8,  4,  4,  8,  4,  // 3
        4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,  // 4
        4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,  // 5
        4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,  // 6
        8,  8,  8,  8,  8,  8,  4,  8,  4,  4,  4,  4,  4,  4,  8,  4,  // 7
        4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,  // 8
        4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,  // 9
        4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,  // A
        4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,  // B
        8,  12, 12, 16, 12, 16, 8,  16, 8,  16, 12, 4,  12, 24, 8,  16, // C
        8,  12, 12, 0,  12, 16, 8,  16, 8,  16, 12, 0,  12, 0,  8,  16, // D
        12, 12, 8,  0,  0,  16, 8,  16, 16, 4,  16, 0,  0,  0,  8,  16, // E
        12, 12, 8,  4,  0,  16, 8,  16, 12, 8,  16, 4,  0,  0,  8,  16  // F
    };
        
};

#endif