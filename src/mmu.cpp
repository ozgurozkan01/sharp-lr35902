#include "../include/mmu.h"
#include "../include/timer.h"
#include "../include/interrupt_controller.h"
#include <fstream>

MMU::MMU(InterruptController &interrupt_controller, Timer &timer) noexcept :
    timer(timer),
    interrupt_controller(interrupt_controller)
{}

void MMU::write(uint16_t address, uint8_t value) noexcept {
    if (address == 0xFF02) {
        memory[address] = value; 

        bool is_transfer_enabled = value & 0x80;
        bool is_master = value & 0x01;

        if (is_master && is_transfer_enabled)
        {
            char c = memory[0xFF01];
            std::cout << c;
            std::cout.flush();

            memory[address] &= ~0x80;
        }
        return;
    }
    
    if (address >= 0xFF04 && address <= 0xFF07) {
        timer.write(address, value);
        return;
    }

    if (address == 0xFF0F) {
        interrupt_controller.write_if(value);
        return;
    }

    if (address == 0xFFFF) {
        interrupt_controller.write_ie(value);
        return;
    }

    memory[address] = value;
}

uint8_t MMU::read(uint16_t address) noexcept {
    if (address >= 0xFF04 && address <= 0xFF07) {
        return timer.read(address);
    }
    
    if (address == 0xFF0F) {
        return interrupt_controller.read_if();
    }

    if (address == 0xFFFF) {
        return interrupt_controller.read_ie();
    }

    return memory[address];
}

bool MMU::load_rom(std::string path) noexcept {
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