#include "../include/cpu.h"
#include "../include/mmu.h"
#include "../include/timer.h"
#include "../debugger/disassembler.h"
#include "../debugger/debugger.h"
#include "../include/interrupt_controller.h"

#include <iostream>
#include <stdint.h>

#define DEBUGGER_MODE false

int main(int argc, char* argv[]) {

    if (argc < 2)
    {
        std::cerr << "You did not enter rom path correctly !" << std::endl;
        return -1;
    }
    
    std::string rom_path = argv[1];

    Timer timer;
    InterruptController interrupt_controller;

    MMU mmu(interrupt_controller, timer);

    if (!mmu.load_rom(rom_path)) {
        return -1;
    }
    
    CPU cpu(mmu, interrupt_controller, timer);
    Disassembler disassembler(mmu);
    Debugger debugger(cpu, mmu, timer, interrupt_controller, disassembler);

    bool is_running = true;

    while (is_running)
    {
    #if DEBUGGER_MODE
        debugger.print();
    #endif
        cpu.execute_instructions();
    }

    return 0;
}