#ifndef MMU_H
#define MMU_H

#include <iostream>
#include <stdint.h>
#include <string>

constexpr uint32_t MEMORY_SIZE = 65536;

class Timer;
class InterruptController;

class MMU {
public:

    MMU(InterruptController &interrupt_controller, Timer &timer) noexcept;

    [[nodiscard]] bool load_rom(std::string rom_path) noexcept;

    void write(uint16_t address, uint8_t value) noexcept;
    uint8_t read(uint16_t address) noexcept;

private:
    uint8_t memory[MEMORY_SIZE] = {};
    
    InterruptController &interrupt_controller;
    Timer &timer;
};

#endif