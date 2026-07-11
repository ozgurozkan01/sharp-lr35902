#ifndef MMU_H
#define MMU_H

#include <iostream>
#include <stdint.h>

const uint32_t MEMORY_SIZE = 65535;

class MMU {
public:

    MMU();

    bool load_rom(std::string rom_path);

private:
    uint8_t memory[MEMORY_SIZE];

};

#endif