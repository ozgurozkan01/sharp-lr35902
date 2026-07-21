# Sharp-LR35902

A CPU emulator for the Sharp-LR35902, the processor at the core of the original Game Boy (DMG-01), written in C++. This project focuses on instruction accuracy, cycle-accurate timing, and robust interrupt execution.

## Features
- **CPU Emulation:** Implementation of the Sharp-LR35902 instruction set.
- **Interrupt Controller:** Emulation of `IE` (Interrupt Enable), `IF` (Interrupt Flag), and `IME` (Interrupt Master Enable) behaviors.
- **Timer System:** Basic emulation of Game Boy timer registers (`DIV`, `TIMA`, `TMA`, `TAC`).

## Test Results

The emulator currently passes Blargg's instruction tests and the instruction timing suite:

```text
.\main.exe roms/01-special.gb                PASSED
.\main.exe roms/02-interrupts                PASSED
.\main.exe roms/03-op sp,hl                  PASSED
.\main.exe roms/04-op r,imm                  PASSED
.\main.exe roms/05-op rp                     PASSED
.\main.exe roms/06-ld r,r                    PASSED
.\main.exe roms/07-jr,jp,call,ret,rst        PASSED
.\main.exe roms/08-misc instr                PASSED
.\main.exe roms/09-op r,r                    PASSED
.\main.exe roms/10-bit ops                   PASSED
.\main.exe roms/11-op a,(hl)                 PASSED
.\main.exe roms/cpu-instr                    PASSED
.\main.exe roms/instr-timing                 PASSED
```

### Current Focus (Work in Progress)
- **`mem-timing`**: Currently transitioning from instruction-level timing to M-cycle (4 T-cycles) accurate memory timing to align the memory reads and writes with the internal timer ticks [INDEX].


### Building
You can compile the project using your preferred compiler. For example, using `g++`:

```bash
g++ .\src\*.cpp -o main
```
### Running Test ROMs
To run any compatible test ROM:

```bash
.\main.exe <rom_path>
```