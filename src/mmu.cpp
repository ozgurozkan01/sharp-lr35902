#include "../include/mmu.h"
#include <fstream>

MMU::MMU() {

}

void MMU::write(uint16_t address, uint8_t value) {
    memory[address] = value;
}

uint8_t MMU::read(uint16_t address) {
    return memory[address];
}

bool MMU::load_rom(std::string path) {
    std::ifstream rom(path, std::ios::binary | std::ios::ate);

    if (!rom.is_open()) {
        std::cerr << "ERROR: ROM file could not be opened!: " << path << std::endl;
        return false;
    }

    size_t rom_size = rom.tellg();
    rom.seekg(0, std::ios::beg);

    uint16_t start_address = 0x000;
    if (start_address + rom_size > MEMORY_SIZE) {
        std::cerr << "ERROR: Exceeds the ROM memory size!" << std::endl;
        return false;
    }

    if (!rom.read(reinterpret_cast<char*>(&memory[start_address]), rom_size)) {
        std::cerr << "ERROR: ROM could not be read!" << std::endl;
        return false;
    }

        std::cout << "SUCCESFULLY: " << path << " (" << rom_size << " bayt) loaded to " << std::hex << start_address << " address." << std::endl;
    return true;
}