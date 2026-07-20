#include "../include/timer.h"
#include "../include/mmu.h"

void Timer::tick(uint8_t cycle, MMU &mmu) noexcept {
    div_counter += cycle;

    if (div_counter >= 256)
    {
        div++;
        div_counter = div_counter % 256;
    }
    
    if (is_timer_enabled())
    {
        tima_counter += cycle;
        uint16_t tima_threshold = get_tima_threshold();

        while (tima_counter >= tima_threshold)
        {
            tima_counter -= tima_threshold;
            
            if (tima == 0xFF) {
                tima = tma;
                
                uint8_t interrupt_flag = mmu.read(0xFF0F);
                mmu.write(0xFF0F, interrupt_flag | 0x04); 
            } 
            
            else {
                tima++;
            }
        }
    }
}

uint16_t Timer::get_tima_threshold() const noexcept {
    uint8_t clock_select = tac & 0x3;

    switch (clock_select)
    {
        case 0x00: return 1024;
        case 0x01: return 16;
        case 0x02: return 64;
        case 0x03: return 256;
        
        default: return 1024 ;
    }
} 

bool Timer::is_timer_enabled() const noexcept {
    return (tac & 0x04) != 0;
}

void Timer::write(uint16_t address, uint8_t value) noexcept {
    switch (address)
    {
        case 0xff04: div = 0; div_counter = 0; break;
        case 0xff05: tima = value; break;
        case 0xff06: tma = value; break;
        case 0xff07: tac = value; break;
        default: break;
    }
}

uint8_t Timer::read(uint16_t address) const noexcept {
    switch (address)
    {
        case 0xff04: return div;
        case 0xff05: return tima;
        case 0xff06: return tma;
        case 0xff07: return tac;
        default: return 0xFF;
    }
}
