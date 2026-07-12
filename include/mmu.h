#ifndef MMU_H
#define MMU_H

#include <iostream>
#include <stdint.h>
#include <string>

constexpr uint32_t MEMORY_SIZE = 65536;

class MMU {
public:

    MMU() noexcept;

    [[nodiscard]] bool load_rom(std::string rom_path) noexcept;

    void write(uint16_t address, uint8_t value) noexcept;
    uint8_t read(uint16_t address) noexcept;

private:
    uint8_t memory[MEMORY_SIZE] = {};
};

#endif