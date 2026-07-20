#ifndef CPU_H
#define CPU_H

#include <iostream>
#include <stdint.h>


typedef union {
    uint16_t word;
        
    struct {
        uint8_t low;
        uint8_t high;
    } bytes;

} RegisterPair;

class MMU;
class Timer;
class InterruptController;

class CPU {
public:

    CPU(MMU &mmu, InterruptController &interrupt_controller, Timer &timer) noexcept;

    void execute_instructions() noexcept;
    void execute_cb_instructions() noexcept;
    void execute_interrupts() noexcept;

    enum class Flag : uint8_t {
        carry       = 0x10,
        half_carry  = 0x20,
        subtraction = 0x40,
        zero        = 0x80
    };

private:
    void print_debug() noexcept;
    void update_clock_cycles(uint8_t cycle) noexcept;

    void set_flag(Flag flag) noexcept;
    void reset_flag(Flag flag) noexcept;
    [[nodiscard]] bool get_flag(Flag flag) const noexcept;

    void adc(uint8_t value) noexcept;
    void add(uint8_t value) noexcept;
    void add(uint16_t value) noexcept;

    void bitwise_and(uint8_t value) noexcept;
    void bitwise_or(uint8_t value) noexcept; 
    void bitwise_xor(uint8_t value) noexcept;    

    void call() noexcept;
    void call_cc(bool condition) noexcept;
    void ccf() noexcept;
    void cp(uint8_t value) noexcept;
    void cpl() noexcept;

    void inc(uint8_t &reg8) noexcept;
    void inc(uint16_t &reg16) noexcept;

    void daa() noexcept;
    void dec(uint8_t &reg8) noexcept;
    void dec(uint16_t &reg16) noexcept;

    void jp() noexcept;
    void jp_cc(bool condition) noexcept;
    void jr() noexcept;
    void jr_cc(bool condition) noexcept;

    void ld(uint16_t &reg16) noexcept;
    void ld(uint8_t &reg8) noexcept;
    void ld(uint8_t &target, uint8_t value) noexcept;
    void ld(uint16_t address, uint8_t value) noexcept;

    void ldh_read(uint8_t offset) noexcept;
    void ldh_write(uint8_t offset) noexcept;

    void push(const uint16_t address) noexcept;
    uint16_t pop() noexcept;

    void ret() noexcept;
    void ret_cc(bool condition) noexcept;
    void reti() noexcept;
    void rla() noexcept;
    void rlca() noexcept;
    void rra() noexcept;
    void rrca() noexcept;
    void rst(uint8_t vector) noexcept;
    
    void sbc(uint8_t value) noexcept;
    void scf() noexcept;
    void sub(uint8_t value) noexcept;

    // Extended Instructions
    void bit_test(uint8_t u3, uint8_t reg8) noexcept;
    void bit_reset(uint8_t u3, uint8_t &reg8) noexcept;
    void bit_set(uint8_t u3, uint8_t &reg8) noexcept;

    void rl(uint8_t &reg8) noexcept;
    void rlc(uint8_t &reg8) noexcept;
    void rr(uint8_t &reg8) noexcept;
    void rrc(uint8_t &reg8) noexcept;

    void sla(uint8_t &reg8) noexcept;
    void sra(uint8_t &reg8) noexcept;
    void srl(uint8_t &value) noexcept;
    void swap(uint8_t &reg8) noexcept;

    bool is_halted;

    RegisterPair af;
    RegisterPair bc;
    RegisterPair de;   
    RegisterPair hl;

    uint16_t sp;
    uint16_t pc;

    MMU &mmu;
    Timer &timer;
    InterruptController &interrupt_controller;
    
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

    const uint8_t cpu_cb_clock_cycles[256] = {
    //  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
        8,  8,  8,  8,  8,  8,  16, 8,  8,  8,  8,  8,  8,  8,  16, 8,  // 0
        8,  8,  8,  8,  8,  8,  16, 8,  8,  8,  8,  8,  8,  8,  16, 8,  // 1
        8,  8,  8,  8,  8,  8,  16, 8,  8,  8,  8,  8,  8,  8,  16, 8,  // 2
        8,  8,  8,  8,  8,  8,  16, 8,  8,  8,  8,  8,  8,  8,  16, 8,  // 3
        8,  8,  8,  8,  8,  8,  12, 8,  8,  8,  8,  8,  8,  8,  12, 8,  // 4
        8,  8,  8,  8,  8,  8,  12, 8,  8,  8,  8,  8,  8,  8,  12, 8,  // 5
        8,  8,  8,  8,  8,  8,  12, 8,  8,  8,  8,  8,  8,  8,  12, 8,  // 6
        8,  8,  8,  8,  8,  8,  12, 8,  8,  8,  8,  8,  8,  8,  12, 8,  // 7
        8,  8,  8,  8,  8,  8,  16, 8,  8,  8,  8,  8,  8,  8,  16, 8,  // 8
        8,  8,  8,  8,  8,  8,  16, 8,  8,  8,  8,  8,  8,  8,  16, 8,  // 9
        8,  8,  8,  8,  8,  8,  16, 8,  8,  8,  8,  8,  8,  8,  16, 8,  // A
        8,  8,  8,  8,  8,  8, 16, 8,  8,  8,  8,  8,  8,  8,  16, 8,  // B
        8,  8,  8,  8,  8,  8,  16, 8,  8,  8,  8,  8,  8,  8,  16, 8,  // C
        8,  8,  8,  8,  8,  8,  16, 8,  8,  8,  8,  8,  8,  8,  16, 8,  // D
        8,  8,  8,  8,  8,  8,  16, 8,  8,  8,  8,  8,  8,  8,  16, 8,  // E
        8,  8,  8,  8,  8,  8,  16, 8,  8,  8,  8,  8,  8,  8,  16, 8   // F
    };
};

#endif