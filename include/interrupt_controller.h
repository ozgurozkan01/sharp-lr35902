#ifndef INTERRUPT_CONTROLLER_H
#define INTERRUPT_CONTROLLER_H

#include <iostream>
#include <stdint.h>

enum class InterruptTypes : uint8_t {
    v_blank = 0,
    lcd     = 1,
    timer   = 2, 
    serial  = 3,
    joypad  = 4
};

class InterruptController {
public:
    explicit InterruptController() noexcept;

    uint8_t get_interrupt_vector(uint8_t interrupt_bit) noexcept;

    void request_interrupt(InterruptTypes type) noexcept;
    [[nodiscard]] bool has_pending_interrupt() const noexcept;
    int acknowledge_interrupt() noexcept;

    uint8_t read_ie() const noexcept;
    void write_ie(uint8_t val) noexcept;

    uint8_t read_if() const noexcept;
    void write_if(uint8_t val) noexcept;

    [[nodiscard]] bool get_ime() const noexcept;
    void set_ime() noexcept;
    void reset_ime() noexcept;

    [[nodiscard]] bool get_pending_ei() const noexcept;
    void set_pending_ei() noexcept;
    void reset_pending_ei() noexcept;

    private:
    uint8_t interrupt_enable;
    uint8_t interrupt_flag;

    bool interrupt_master_enable;
    bool pending_interrupt_enable;
};

#endif