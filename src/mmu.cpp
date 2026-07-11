#include "../include/mmu.h"
#include <fstream>

/* 
    Start	End	    Description	Notes
    0000	3FFF	16 KiB ROM bank 00	From cartridge, usually a fixed bank
    4000	7FFF	16 KiB ROM Bank 01–NN	From cartridge, switchable bank via mapper (if any)
    8000	9FFF	8 KiB Video RAM (VRAM)	In CGB mode, switchable bank 0/1
    A000	BFFF	8 KiB External RAM	From cartridge, switchable bank if any
    C000	CFFF	4 KiB Work RAM (WRAM)	
    D000	DFFF	4 KiB Work RAM (WRAM)	In CGB mode, switchable bank 1–7
    E000	FDFF	Echo RAM (mirror of C000–DDFF)	Nintendo says use of this area is prohibited.
    FE00	FE9F	Object attribute memory (OAM)	
    FEA0	FEFF	Not Usable	Nintendo says use of this area is prohibited.
    FF00	FF7F	I/O Registers	
    FF80	FFFE	High RAM (HRAM)	
    FFFF	FFFF	Interrupt Enable register (IE)	
*/

MMU::MMU() {

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