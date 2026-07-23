#ifndef TIMER_H
#define TIMER_H

#include <iostream>
#include <stdint.h>

class MMU;

class Timer {
public:
    Timer() = default;
    
    void tick(uint8_t cycle, MMU &mmu) noexcept;

    void write(uint16_t address, uint8_t value) noexcept;
    [[nodiscard]] uint8_t read(uint16_t address) const noexcept;
private:

    uint16_t get_tima_threshold() const noexcept; 

    [[nodiscard]] bool is_timer_enabled() const noexcept;


    uint16_t div_counter{};
    uint16_t tima_counter{};

    uint8_t div{};
    uint8_t tima{};
    uint8_t tma{};
    uint8_t tac{};
};

#endif