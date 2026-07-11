#include "../include/cpu.h"

#include <iostream>
#include <stdint.h>

int main() {

    MMU mmu;

    if (!mmu.load_rom("roms/01-special.gb")) {
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