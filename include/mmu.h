#ifndef MMU_H
#define MMU_H

#include <iostream>
#include <stdint.h>
#include <string>

const uint32_t MEMORY_SIZE = 65536;

class MMU {
public:

    MMU();

    bool load_rom(std::string rom_path);

    void write(uint16_t address, uint8_t value);
    uint8_t read(uint16_t address);

private:
    uint8_t memory[MEMORY_SIZE] = {};
};

#endif