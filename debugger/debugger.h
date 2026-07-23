#ifndef DEBUGGER_H
#define DEBUGGER_H

class CPU;
class MMU;
class Timer;
class InterruptController;
class Disassembler;

class Debugger {
public:
    explicit Debugger(CPU &cpu, MMU &mmu, Timer &timer, InterruptController &interrupt_controller, Disassembler &disassembler) noexcept;
    void print() noexcept;
private:

    CPU &cpu;
    MMU &mmu;
    Timer &timer;
    InterruptController &interrupt_controller;
    Disassembler &disassembler;
};

#endif