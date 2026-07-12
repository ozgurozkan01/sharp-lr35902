#include "../include/cpu.h"

#include <iostream>
#include <stdint.h>

int main(int argc, char* argv[]) {

    if (argc < 2)
    {
        std::cerr << "You did not enter rom path correctly !" << std::endl;
        return -1;
    }
    
    std::string rom_path = argv[1];

    MMU mmu;

    if (!mmu.load_rom(rom_path)) {
        return -1;
    }
    
    CPU cpu(mmu);

    bool is_running = true;

    while (is_running)
    {
        cpu.execute_instructions();
    }
    

    return 0;
}