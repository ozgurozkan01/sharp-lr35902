#include "../include/interrupt_controller.h"

InterruptController::InterruptController() :
    interrupt_enable(0x00),
    interrupt_flag(0xE1),
    interrupt_master_enable(false),
    pending_interrupt_enable(false)
{}

void InterruptController::request_interrupt(InterruptTypes type) noexcept {
    interrupt_flag |= (1 << static_cast<uint8_t>(type));
}

bool InterruptController::has_pending_interrupt() const noexcept {
    return (interrupt_enable & interrupt_flag & 0x1F) != 0;
}

int InterruptController::acknowledge_interrupt() noexcept {
    uint8_t pending = interrupt_enable & interrupt_flag & 0x1F;

    for (int i = 0; i < 5; ++i) {
        if (pending & (1 << i)) {
            interrupt_flag &= ~(1 << i);
            return i;
        }
    }

    return -1;
}

uint8_t InterruptController::get_interrupt_vector(uint8_t interrupt_bit) noexcept {
    switch (interrupt_bit)
    {
        case 0: return 0x40; // VBlank
        case 1: return 0x48; // LCD
        case 2: return 0x50; // Timer
        case 3: return 0x58; // Serial
        case 4: return 0x60; // Joypad
    }

    return 0x0;
}

uint8_t InterruptController::read_ie() const noexcept { return interrupt_enable; }
void InterruptController::write_ie(uint8_t val) noexcept { interrupt_enable = val & 0x1F; }

uint8_t InterruptController::read_if() const noexcept { return interrupt_flag; }
void InterruptController::write_if(uint8_t val) noexcept { interrupt_flag = (val & 0x1F) | 0xE0; }

bool InterruptController::get_ime() const noexcept { return interrupt_master_enable; }
void InterruptController::set_ime() noexcept { interrupt_master_enable = true; }
void InterruptController::reset_ime() noexcept { interrupt_master_enable = false; }

bool InterruptController::get_pending_ei() const noexcept { return pending_interrupt_enable; }
void InterruptController::set_pending_ei() noexcept { pending_interrupt_enable = true; }
void InterruptController::reset_pending_ei() noexcept { pending_interrupt_enable = false; }