#include "../include/cpu.h"
#include <iomanip>

#define DEBUG_MOD false

CPU::CPU(MMU &mmu) noexcept :
    mmu(mmu)
{
    is_interrupt_enabled = true;

    af.word = 0x01B0;
    bc.word = 0x0013;
    de.word = 0x00d8;
    hl.word = 0x014d;
    pc = 0x100;
    sp = 0xfffe;

    total_cycle = 0;
}

void CPU::execute_instructions() noexcept {
    
    const uint8_t opcode = mmu.read(pc);
    uint8_t cycle = cpu_clock_cycles[opcode];
    

#if DEBUG_MOD
    print_debug();
#endif

    update_clock_cycles(cycle);

    switch (opcode)
    {
        case 0x0:
            pc++;
            break;
        case 0x01:
            ld(bc.word);
            break;
        case 0x03:
            inc(bc.word);
            break;
        case 0x04:
            inc(bc.bytes.high);
            break;
        case 0x05:
            dec(bc.bytes.high);
            break;
        case 0x06:
            ld(bc.bytes.high);
            break;
        case 0x07:
            rlca();
            break;
        case 0x08:
        {
            uint8_t low = mmu.read(pc + 1);
            uint8_t high = mmu.read(pc + 2);
            uint16_t address = (high << 8) | low;
        
            uint8_t sp_low = sp & 0xFF;
            uint8_t sp_high = sp >> 8;

            ld(address, sp_low);
            ld(address + 1, sp_high);
            
            pc++;
            break;
        }
        case 0x09:
            add(bc.word);
            break;
        case 0x0a:
            ld(af.bytes.high, mmu.read(bc.word));
            break;
        case 0x0b:
            dec(bc.word);
            break;
        case 0x0c:
            inc(bc.bytes.low);
            break;
        case 0x0d:
            dec(bc.bytes.low);
            break;
        case 0x0e:
            ld(bc.bytes.low);
            break;
        case 0x0f:
            rrca();
            break;
        case 0x10:
            pc += 2; // STOP
            break;
        case 0x11:
            ld(de.word);
            break;
        case 0x12:
            ld(de.word, af.bytes.high);
            break;
        case 0x13:
            inc(de.word);
            break;
        case 0x14:
            inc(de.bytes.high);
            break;
        case 0x15:
            dec(de.bytes.high);
            break;
        case 0x16:
            ld(de.bytes.high, mmu.read(pc + 1));
            pc++;
            break;
        case 0x17:
            rla();
            break;
        case 0x18:
            jr();
            break;
        case 0x19:
            add(de.word);
            break;
        case 0x1a:
            ld(af.bytes.high, mmu.read(de.word));
            break;
        case 0x1b:
            dec(de.word);
            break;
        case 0x1c:
            inc(de.bytes.low);
            break;
        case 0x1d:
            dec(de.bytes.low);
            break;
        case 0x1e:
            ld(de.bytes.low);
            break;
        case 0x1f:
            rra();
            break;
        case 0x20:
            jr_cc(!get_flag(Flag::zero));
            break;
        case 0x21:
            ld(hl.word);
            break;
        case 0x22:
            ld(hl.word++, af.bytes.high);
            break;
        case 0x23:
            inc(hl.word);
            break;
        case 0x24:
            inc(hl.bytes.high);
            break;
        case 0x25:
            dec(hl.bytes.high);
            break;
        case 0x26:
            ld(hl.bytes.high, mmu.read(pc + 1));
            pc++;
            break;
        case 0x27:
            daa();
            break;
        case 0x28:
            jr_cc(get_flag(Flag::zero));
            break;
        case 0x29:
            add(hl.word);
            break;
        case 0x2a:
            ld(af.bytes.high, mmu.read(hl.word++));
            break;
        case 0x2b:
            dec(hl.word);
            break;
        case 0x2c:
            inc(hl.bytes.low);
            break;
        case 0x2d:
            dec(hl.bytes.low);
            break;
        case 0x2e:
            ld(hl.bytes.low);
            break;
        case 0x2f:
            cpl();
            break;
        case 0x30:
            jr_cc(!get_flag(Flag::carry));
            break;
        case 0x31:
            ld(sp);
            break;
        case 0x32:
            ld(hl.word--, af.bytes.high);
            break;
        case 0x33:
            inc(sp);
            break;
        case 0x35: 
        {
            uint8_t val = mmu.read(hl.word);
            
            dec(val); 

            mmu.write(hl.word, val);
            break;
        }
        case 0x36:
            ld(hl.word, mmu.read(pc + 1));
            pc++;
            break;
        case 0x37:
            scf();
            break;
        case 0x38:
            jr_cc(get_flag(Flag::carry));
            break;
        case 0x39:
            add(sp);
            break;
        case 0x3b:
            dec(sp);
            break;
        case 0x3c:
            inc(af.bytes.high);
            break;
        case 0x3d:
            dec(af.bytes.high);
            break;
        case 0x3e:
            ld(af.bytes.high);
            break;
        case 0x3f:
            ccf();
            break;
        case 0x40:
            ld(bc.bytes.high, bc.bytes.high);
            break;
        case 0x41:
            ld(bc.bytes.high, bc.bytes.low);
            break;
        case 0x42:
            ld(bc.bytes.high, de.bytes.high);
            break;
        case 0x43:
            ld(bc.bytes.high, de.bytes.low);
            break;
        case 0x44:
            ld(bc.bytes.high, hl.bytes.high);
            break;
        case 0x45:
            ld(bc.bytes.high, hl.bytes.low);
            break;
        case 0x46:
            ld(bc.bytes.high, mmu.read(hl.word));
            break;
        case 0x47:
            ld(bc.bytes.high, af.bytes.high);
            break;
        case 0x48:
            ld(bc.bytes.low, bc.bytes.high);
            break;
        case 0x49:
            ld(bc.bytes.low, bc.bytes.low);
            break;
        case 0x4a:
            ld(bc.bytes.low, de.bytes.high);
            break;
        case 0x4b:
            ld(bc.bytes.low, de.bytes.low);
            break;
        case 0x4c:
            ld(bc.bytes.low, hl.bytes.high);
            break;
        case 0x4d:
            ld(bc.bytes.low, hl.bytes.low);
            break;
        case 0x4e:
            ld(bc.bytes.low, mmu.read(hl.word));
            break;
        case 0x4f:
            ld(bc.bytes.low, af.bytes.high);
            break;
        case 0x50:
            ld(de.bytes.high, bc.bytes.high);
            break;
        case 0x51:
            ld(de.bytes.high, bc.bytes.low);
            break;
        case 0x52:
            ld(de.bytes.high, de.bytes.high);
            break;
        case 0x53:
            ld(de.bytes.high, de.bytes.low);
            break;
        case 0x54:
            ld(de.bytes.high, hl.bytes.high);
            break;
        case 0x55:
            ld(de.bytes.high, hl.bytes.low);
            break;
        case 0x56:
            ld(de.bytes.high, mmu.read(hl.word));
            break;
        case 0x57:
            ld(de.bytes.high, af.bytes.high);
            break;
        case 0x58:
            ld(de.bytes.low, bc.bytes.high);
            break;
        case 0x59:
            ld(de.bytes.low, bc.bytes.low);
            break;
        case 0x5a:
            ld(de.bytes.low, de.bytes.high);
            break;
        case 0x5b:
            ld(de.bytes.low, de.bytes.low);
            break;
        case 0x5c:
            ld(de.bytes.low, hl.bytes.high);
            break;
        case 0x5d:
            ld(de.bytes.low, hl.bytes.low);
            break;
        case 0x5e:
            ld(de.bytes.low, mmu.read(hl.word));
            break;
        case 0x5f:
            ld(de.bytes.low, af.bytes.high);
            break;
        case 0x60:
            ld(hl.bytes.high, bc.bytes.high);
            break;
        case 0x61:
            ld(hl.bytes.high, bc.bytes.low);
            break;
        case 0x62:
            ld(hl.bytes.high, de.bytes.high);
            break;
        case 0x63:
            ld(hl.bytes.high, de.bytes.low);
            break;
        case 0x64:
            ld(hl.bytes.high, hl.bytes.high);
            break;
        case 0x65:
            ld(hl.bytes.high, hl.bytes.low);
            break;
        case 0x66:
            ld(hl.bytes.high, mmu.read(hl.word));
            break;
        case 0x67:
            ld(hl.bytes.high, af.bytes.high);
            break;
        case 0x68:
            ld(hl.bytes.low, bc.bytes.high);
            break;
        case 0x69:
            ld(hl.bytes.low, bc.bytes.low);
            break;
        case 0x6a:
            ld(hl.bytes.low, de.bytes.high);
            break;
        case 0x6b:
            ld(hl.bytes.low, de.bytes.low);
            break;
        case 0x6c:
            ld(hl.bytes.low, hl.bytes.high);
            break;
        case 0x6d:
            ld(hl.bytes.low, hl.bytes.low);
            break;
        case 0x6e:
            ld(hl.bytes.low, mmu.read(hl.word));
            break;
        case 0x6f:
            ld(hl.bytes.low, af.bytes.high);
            break;
        case 0x70:
            ld(hl.word, bc.bytes.high);
            break;
        case 0x71:
            ld(hl.word, bc.bytes.low);
            break;
        case 0x72:
            ld(hl.word, de.bytes.high);
            break;
        case 0x73:
            ld(hl.word, de.bytes.low);
            break;
        case 0x74:
            ld(hl.word, hl.bytes.high);
            break;
        case 0x75:
            ld(hl.word, hl.bytes.low);
            break;
        case 0x77:
            ld(hl.word, af.bytes.high);
            break;
        case 0x78:
            ld(af.bytes.high, bc.bytes.high);
            break;
        case 0x79:
            ld(af.bytes.high, bc.bytes.low);
            break;
        case 0x7a:  
            ld(af.bytes.high, de.bytes.high);
            break;
        case 0x7b:
            ld(af.bytes.high, de.bytes.low);
            break;
        case 0x7c:
            ld(af.bytes.high, hl.bytes.high);
            break;
        case 0x7d:
            ld(af.bytes.high, hl.bytes.low);
            break;
        case 0x7e:
            ld(af.bytes.high, mmu.read(hl.word));
            break;
        case 0x7f:
            ld(af.bytes.high, af.bytes.high);
            break;
        case 0x80:
            add(bc.bytes.high);
            break;
        case 0x81:
            add(bc.bytes.low);
            break;
        case 0x82:
            add(de.bytes.high);
            break;
        case 0x83:
            add(de.bytes.low);
            break;
        case 0x84:
            add(hl.bytes.high);
            break;
        case 0x85:
            add(hl.bytes.low);
            break;
        case 0x87:
            add(af.bytes.high);
            break;
        case 0x88:
            adc(bc.bytes.high);
            break;
        case 0x89:
            adc(bc.bytes.low);
            break;
        case 0x8a:
            adc(de.bytes.high);
            break;
        case 0x8b:
            adc(de.bytes.low);
            break;
        case 0x8c:
            adc(hl.bytes.high);
            break;
        case 0x8d:
            adc(hl.bytes.low);
            break;
        case 0x8f:
            adc(af.bytes.high);
            break;
        case 0x90:
            sub(bc.bytes.high);
            break;
        case 0x91:
            sub(bc.bytes.low);
            break;
        case 0x92:
            sub(de.bytes.high);
            break;
        case 0x93:
            sub(de.bytes.low);
            break;
        case 0x94:
            sub(hl.bytes.high);
            break;
        case 0x95:
            sub(hl.bytes.low);
            break;
        case 0x97:
            sub(af.bytes.high);
            break;
        case 0x98:
            sbc(bc.bytes.high);
            break;
        case 0x99:
            sbc(bc.bytes.low);
            break;
        case 0x9a:
            sbc(de.bytes.high);
            break;
        case 0x9b:
            sbc(de.bytes.low);
            break;
        case 0x9c:
            sbc(hl.bytes.high);
            break;
        case 0x9d:
            sbc(hl.bytes.low);
            break;
        case 0x9f:
            sbc(af.bytes.high);
            break;
        case 0xa0:
            bitwise_and(bc.bytes.high);
            break;
        case 0xa1:
            bitwise_and(bc.bytes.low);
            break;
        case 0xa2:
            bitwise_and(de.bytes.high);
            break;
        case 0xa3:
            bitwise_and(de.bytes.low);
            break;
        case 0xa4:
            bitwise_and(hl.bytes.high);
            break;
        case 0xa5:
            bitwise_and(hl.bytes.low);
            break;
        case 0xa7:
            bitwise_and(af.bytes.high);
            break;
        case 0xa8:
            bitwise_xor(bc.bytes.high);
            break;
        case 0xa9:
            bitwise_xor(bc.bytes.low);
            break;
        case 0xaa:
            bitwise_xor(de.bytes.high);
            break;
        case 0xab:
            bitwise_xor(de.bytes.low);
            break;
        case 0xac:
            bitwise_xor(hl.bytes.high);
            break;
        case 0xad:
            bitwise_xor(hl.bytes.low);
            break;
        case 0xae:
            bitwise_xor(mmu.read(hl.word));
            break;
        case 0xaf:
            bitwise_xor(af.bytes.high);
            break;
        case 0xb0:
            bitwise_or(bc.bytes.high);
            break;
        case 0xb1:
            bitwise_or(bc.bytes.low);
            break;
        case 0xb2:
            bitwise_or(de.bytes.high);
            break;
        case 0xb3:
            bitwise_or(de.bytes.low);
            break;
        case 0xb4:
            bitwise_or(hl.bytes.high);
            break;
        case 0xb5:
            bitwise_or(hl.bytes.low);
            break;
        case 0xb6:
            bitwise_or(mmu.read(hl.word));
            break;
        case 0xb7:
            bitwise_or(af.bytes.high);
            break;
        case 0xb8:
            cp(bc.bytes.high);
            break;
        case 0xb9:
            cp(bc.bytes.low);
            break;
        case 0xba:
            cp(de.bytes.high);
            break;
        case 0xbb:
            cp(de.bytes.low);
            break;
        case 0xbc:
            cp(hl.bytes.high);
            break;
        case 0xbd:
            cp(hl.bytes.low);
            break;
        case 0xbf:
            cp(af.bytes.high);
            break;
        case 0xc0:
            ret_cc(!get_flag(Flag::zero));
            break;
        case 0xc1:
            bc.word = pop();
            break;
        case 0xc2:
            jp_cc(!get_flag(Flag::zero));
            break;
        case 0xc3:
            jp();
            break;
        case 0xc4:
            call_cc(!get_flag(Flag::zero));
            break;
        case 0xc5:
            push(bc.word);
            break;
        case 0xc6:
            add(mmu.read(pc + 1));
            pc++;
            break;
        case 0xc7:
            rst(0);
            break;
        case 0xc8:
            ret_cc(get_flag(Flag::zero));
            break;
        case 0xc9:
            ret();
            break;
        case 0xca:
            jp_cc(get_flag(Flag::zero));
            break;
        case 0xcb:
            execute_cb_instructions();
            break;
        case 0xcc:
            call_cc(get_flag(Flag::zero));
            break;
        case 0xcd:
            call();
            break;
        case 0xce:
            adc(mmu.read(pc + 1));
            pc++;
            break;
        case 0xcf:
            rst(1);
            break;
        case 0xd0:
            ret_cc(!get_flag(Flag::carry));
            break;
        case 0xd1:
            de.word = pop();
            break;
        case 0xd2:
            jp_cc(!get_flag(Flag::carry));
            break;
        case 0xd4:
            call_cc(!get_flag(Flag::carry));
            break;
        case 0xd5:
            push(de.word);
            break;
        case 0xd6:
            sub(mmu.read(pc + 1));
            pc++;
            break;
        case 0xd7:
            rst(2);
            break;
        case 0xd8:
            ret_cc(get_flag(Flag::carry));
            break;
        case 0xd9:
            ret(); // RETI actually
            break;
        case 0xda:
            jp_cc(get_flag(Flag::carry));
            break;
        case 0xdc:
            call_cc(get_flag(Flag::carry));
            break;
        case 0xdf:
            rst(4);
            break;
        case 0xe0:
            ldh_write(mmu.read(pc + 1));
            pc += 2;
            break;
        case 0xe1:
            hl.word = pop();
            break;
        case 0xe5:
            push(hl.word);
            break;
        case 0xe6:
            bitwise_and(mmu.read(pc + 1));
            pc++;
            break;
        case 0xe7:
            rst(5);
            break;
        case 0xe8:
        {
            uint8_t unsigned_data = mmu.read(pc + 1);
            int8_t signed_data = static_cast<int8_t>(unsigned_data);

            uint8_t low_sp = sp & 0xFF;

            ((low_sp & 0x0F) + (unsigned_data & 0x0F) > 0x0F) ? set_flag(Flag::half_carry) : reset_flag(Flag::half_carry); 
            (low_sp + unsigned_data > 0xFF) ? set_flag(Flag::carry) : reset_flag(Flag::carry);

            sp += signed_data;

            reset_flag(Flag::zero);
            reset_flag(Flag::subtraction);

            pc += 2;
            break;
        }
        case 0xe9:
            pc = hl.word;
            break;
        case 0xea:
        {
            uint8_t low = mmu.read(pc + 1);
            uint8_t high = mmu.read(pc + 2);
            uint16_t address = (high << 8) | low;
            ld(address, af.bytes.high);
            pc += 2;
            break;
        }
        case 0xee:
            bitwise_xor(mmu.read(pc + 1));
            pc++;
            break;
        case 0xef:
            rst(5);
            break;
        case 0xf0:
            ldh_read(mmu.read(pc + 1));
            pc += 2;
            break;
        case 0xf1:
            af.word = pop();
            af.bytes.low &= 0xF0;
            break;
        case 0xf3:
            is_interrupt_enabled = false;
            pc++;
            break;
        case 0xf5:
            push(af.word);
            break;
        case 0xf6:
            bitwise_or(mmu.read(pc + 1));
            pc++;
            break;
        case 0xf7:
            rst(6);
            break;
        case 0xf8:
        {
            uint8_t unsigned_data = mmu.read(pc + 1);
            int8_t signed_data = static_cast<int8_t>(unsigned_data);

            uint8_t low_sp = sp & 0xFF;

            ((low_sp & 0x0F) + (unsigned_data & 0x0F) > 0x0F) ? set_flag(Flag::half_carry) : reset_flag(Flag::half_carry); 
            (low_sp + unsigned_data > 0xFF) ? set_flag(Flag::carry) : reset_flag(Flag::carry);

            hl.word = sp + signed_data;

            reset_flag(Flag::zero);
            reset_flag(Flag::subtraction);

            pc += 2;
            break;
        }
        case 0xf9:
            sp = hl.word;
            pc++;
            break;
        case 0xfa:
        {
            uint8_t low = mmu.read(pc + 1);
            uint8_t high = mmu.read(pc + 2);
            uint16_t address = (high << 8) | low;
            uint8_t data = mmu.read(address);

            ld(af.bytes.high, data);
            pc += 2;
            break;
        }
        case 0xfe:
            cp(mmu.read(pc + 1));
            pc++;
            break;
        case 0xff:
            rst(7);
            break;
        default:
            std::cout << "DEFAULT CASE RAN : 0x" << std::hex << (int)opcode << std::endl;
            exit(-1);
    }
}

void CPU::execute_cb_instructions() noexcept {
    uint8_t opcode = mmu.read(pc + 1);

    switch (opcode)
    {
        case 0x00:
            rlc(bc.bytes.high);
            break;
        case 0x01:
            rlc(bc.bytes.low);
            break;
        case 0x02:
            rlc(de.bytes.high);
            break;
        case 0x03:
            rlc(de.bytes.low);
            break;
        case 0x04:
            rlc(hl.bytes.high);
            break;
        case 0x05:
            rlc(hl.bytes.low);
            break;
        case 0x07:
            rlc(af.bytes.high);
            break;
        case 0x08: 
            rrc(bc.bytes.high); 
            break;
        case 0x09: 
            rrc(bc.bytes.low); 
            break;
        case 0x0a: 
            rrc(de.bytes.high); 
            break;
        case 0x0b: 
            rrc(de.bytes.low); 
            break;
        case 0x0c: 
            rrc(hl.bytes.high); 
            break;
        case 0x0d: 
            rrc(hl.bytes.low); 
            break;
        case 0x0f: 
            rrc(af.bytes.high); 
            break;
        case 0x10:
            rl(bc.bytes.high);
            break;
        case 0x11:
            rl(bc.bytes.low);
            break;
        case 0x12: 
            rl(de.bytes.high);
            break;
        case 0x13: 
            rl(de.bytes.low);
            break;
        case 0x14: 
            rl(hl.bytes.high);
            break;
        case 0x15: 
            rl(hl.bytes.low);
            break;
        case 0x17: 
            rl(af.bytes.high);
            break;
        case 0x18:
            rr(bc.bytes.high);
            break;
        case 0x19:
            rr(bc.bytes.low);
            break;
        case 0x1a:
            rr(de.bytes.high);
            break;
        case 0x1b:
            rr(de.bytes.low);
            break;
        case 0x1c:
            rr(hl.bytes.high);
            break;
        case 0x1d:
            rr(hl.bytes.low);
            break;
        case 0x1f:
            rr(af.bytes.high);
            break;
            
        case 0x20: sla(bc.bytes.high); break;
        case 0x21: sla(bc.bytes.low); break;
        case 0x22: sla(de.bytes.high); break;
        case 0x23: sla(de.bytes.low); break;
        case 0x24: sla(hl.bytes.high); break;
        case 0x25: sla(hl.bytes.low); break;
        case 0x27: sla(af.bytes.high); break;

        case 0x28: sra(bc.bytes.high); break;
        case 0x29: sra(bc.bytes.low); break;
        case 0x2a: sra(de.bytes.high); break;
        case 0x2b: sra(de.bytes.low); break;
        case 0x2c: sra(hl.bytes.high); break;
        case 0x2d: sra(hl.bytes.low); break;
        case 0x2f: sra(af.bytes.high); break;

        case 0x30: swap(bc.bytes.high); break;
        case 0x31: swap(bc.bytes.low); break;
        case 0x32: swap(de.bytes.high); break;
        case 0x33: swap(de.bytes.low); break;
        case 0x34: swap(hl.bytes.high); break;
        case 0x35: swap(hl.bytes.low); break;
        case 0x37: swap(af.bytes.high); break;

        case 0x38: srl(bc.bytes.high); break;
        case 0x39: srl(bc.bytes.low); break;
        case 0x3a: srl(de.bytes.high); break;
        case 0x3b: srl(de.bytes.low); break;
        case 0x3c: srl(hl.bytes.high); break;
        case 0x3d: srl(hl.bytes.low); break;
        case 0x3f: srl(af.bytes.high); break;

        default:
            std::cout << "DEFAULT CB CASE RAN : 0x" << std::hex << (int)opcode << std::endl;
            exit(-1);
    }

    pc += 2;
}

void CPU::adc(uint8_t value) noexcept {
    uint8_t old_a = af.bytes.high;
    uint8_t old_carry = get_flag(Flag::carry) ? 1 : 0;
    af.bytes.high = old_a + value + old_carry;
    
    af.bytes.high == 0 ? set_flag(Flag::zero) : reset_flag(Flag::zero);
    reset_flag(Flag::subtraction);
    ((old_a & 0x0f) + (value & 0x0f) + old_carry) > 0x0f ? set_flag(Flag::half_carry) : reset_flag(Flag::half_carry);
    (old_a + value + old_carry) > 0xFF ? set_flag(Flag::carry) : reset_flag(Flag::carry);

    pc++;
}

void CPU::add(uint8_t value) noexcept {
    uint8_t old_a = af.bytes.high;
    af.bytes.high = old_a + value;

    af.bytes.high == 0 ? set_flag(Flag::zero) : reset_flag(Flag::zero);
    reset_flag(Flag::subtraction);
    (old_a & 0x0f) + (value & 0x0f) > 0x0f ? set_flag(Flag::half_carry) : reset_flag(Flag::half_carry);
    (old_a + value) > 0xff ? set_flag(Flag::carry) : reset_flag(Flag::carry);

    pc++;
}

void CPU::add(uint16_t value) noexcept {
    uint16_t old_hl = hl.word;
    hl.word += value;

    reset_flag(Flag::subtraction);
    ((old_hl & 0xfff) + (value & 0xfff)) > 0xfff ? set_flag(Flag::half_carry) : reset_flag(Flag::half_carry);
    (old_hl + value) > 0xFFFF ? set_flag(Flag::carry) : reset_flag(Flag::carry);

    pc++;
}

void CPU::bitwise_or(uint8_t value) noexcept {
    af.bytes.high |= value;

    af.bytes.high ? reset_flag(Flag::zero) : set_flag(Flag::zero);
    reset_flag(Flag::carry);
    reset_flag(Flag::half_carry);
    reset_flag(Flag::subtraction);

    pc++;
}

void CPU::bitwise_and(uint8_t value) noexcept {
    af.bytes.high &= value;

    af.bytes.high == 0 ? set_flag(Flag::zero) : reset_flag(Flag::zero);
    reset_flag(Flag::subtraction);
    set_flag(Flag::half_carry); 
    reset_flag(Flag::carry);
        
    pc++;
}

void CPU::bitwise_xor(uint8_t value) noexcept {
    af.bytes.high ^= value;

    af.bytes.high == 0 ? set_flag(Flag::zero) : reset_flag(Flag::zero);
    reset_flag(Flag::subtraction);
    reset_flag(Flag::half_carry); 
    reset_flag(Flag::carry);
        
    pc++;
}

void CPU::call() noexcept {
    uint8_t low = mmu.read(pc + 1);
    uint8_t high = mmu.read(pc + 2);
    uint16_t jump_address = (high << 8) | low;

    push(pc + 3);

    pc = jump_address;
}

void CPU::call_cc(bool condition) noexcept {
    if (condition)
    {
        call();
        update_clock_cycles(12);
        return;
    }
    pc += 3;
}

void CPU::ccf() noexcept {
    reset_flag(Flag::subtraction);
    reset_flag(Flag::half_carry);
    get_flag(Flag::carry) ? reset_flag(Flag::carry) : set_flag(Flag::carry);

    pc++;
}

void CPU::cp(uint8_t value) noexcept {
    uint8_t result = af.bytes.high - value;

    result == 0 ? set_flag(Flag::zero) : reset_flag(Flag::zero);
    set_flag(Flag::subtraction);
    (value  & 0x0f) > (af.bytes.high & 0x0f) ? set_flag(Flag::half_carry) : reset_flag(Flag::half_carry);
    value > af.bytes.high ? set_flag(Flag::carry) : reset_flag(Flag::carry);

    pc++;
}

void CPU::cpl() noexcept {
    af.bytes.high = ~af.bytes.high;

    set_flag(Flag::subtraction);
    set_flag(Flag::half_carry);

    pc++;
}

void CPU::inc(uint8_t &reg8) noexcept {
    uint8_t old_value = reg8;
    reg8++;
    pc++;

    reg8 == 0 ? set_flag(Flag::zero) : reset_flag(Flag::zero);
    reset_flag(Flag::subtraction);
    (old_value & 0x0f) == 0x0f ? set_flag(Flag::half_carry) : reset_flag(Flag::half_carry);
}

void CPU::inc(uint16_t &reg16) noexcept {
    reg16++;
    pc++;
}

void CPU::daa() noexcept {
    uint8_t adjustment = 0;

    if (get_flag(Flag::subtraction))
    {
        if (get_flag(Flag::half_carry)) { adjustment += 0x06; }
        if (get_flag(Flag::carry))      { adjustment += 0x60; }
        
        af.bytes.high -= adjustment;
    }
    else
    {
        uint8_t a = af.bytes.high;

        if (get_flag(Flag::half_carry) || ((a & 0xF) > 0x09) ) { adjustment += 0x06; }
        if (get_flag(Flag::carry) || a > 0x99)                 { adjustment += 0x60; set_flag(Flag::carry); }
            
        af.bytes.high += adjustment;
    }

    af.bytes.high == 0 ? set_flag(Flag::zero) : reset_flag(Flag::zero);
    reset_flag(Flag::half_carry);

    pc++;
}

void CPU::dec(uint8_t &reg8) noexcept {
    uint8_t old_value = reg8;
    reg8--;
    pc++;

    reg8 == 0 ? set_flag(Flag::zero) : reset_flag(Flag::zero);
    set_flag(Flag::subtraction);
    (old_value & 0x0f) == 0x0 ? set_flag(Flag::half_carry) : reset_flag(Flag::half_carry);
}

void CPU::dec(uint16_t &reg16) noexcept {
    reg16--;
    pc++;
}

void CPU::jp() noexcept {
    uint8_t low = mmu.read(pc + 1);
    uint8_t high = mmu.read(pc + 2);
    pc = (high << 8) | low;
}

void CPU::jp_cc(bool condition) noexcept {
    if (condition)
    {
        jp();
        update_clock_cycles(4);
        return;
    }

    pc += 3;
}


void CPU::jr() noexcept {
    uint8_t unsigned_offset = mmu.read(pc + 1);
    int8_t signed_offset = static_cast<int8_t>(unsigned_offset);
    pc += signed_offset;
    pc += 2;
}

void CPU::jr_cc(bool condition) noexcept {

    if (condition)
    {
        jr();
        update_clock_cycles(4);
        return;
    }
    
    pc += 2;
}

void CPU::ld(uint8_t &reg8) noexcept {
    uint8_t data = mmu.read(pc + 1);
    ld(reg8, data);
    pc++;
}

void CPU::ld(uint16_t &reg16) noexcept {
    uint8_t low = mmu.read(pc + 1);
    uint8_t high = mmu.read(pc + 2);
    reg16 = (high << 8) | low;

    pc += 3;
}

void CPU::ld(uint8_t &target, uint8_t value) noexcept {
    target = value;
    pc++;
}

void CPU::ld(uint16_t address, uint8_t value) noexcept {
    mmu.write(address, value);
    pc++;
}

void CPU::ldh_read(uint8_t offset) noexcept {
    af.bytes.high = mmu.read(0xff00 + offset);
}

void CPU::ldh_write(uint8_t offset) noexcept {
    mmu.write(0xff00 + offset, af.bytes.high);
}

void CPU::push(const uint16_t address) noexcept {
    mmu.write(sp - 1, (address >> 8) & 0xFF);
    mmu.write(sp - 2, address & 0xFF);
    sp -= 2;
    pc++;
}

uint16_t CPU::pop() noexcept {

    uint8_t low = mmu.read(sp);
    uint8_t high = mmu.read(sp + 1);
    uint16_t popped_address = (high << 8) | low;

    sp += 2;
    pc++;

    return popped_address;
}

void CPU::ret() noexcept {
    pc = pop();
}

void CPU::ret_cc(bool condition) noexcept {
    if (condition)
    {
        ret();
        update_clock_cycles(12);
        return;
    }
    pc++;
}

void CPU::rla() noexcept {
    uint8_t msb = af.bytes.high & 0x80;
    uint8_t old_carry = get_flag(Flag::carry) ? 1 : 0;

    af.bytes.high = (af.bytes.high << 1) | old_carry;

    reset_flag(Flag::zero);
    reset_flag(Flag::subtraction);
    reset_flag(Flag::half_carry);
    msb ? set_flag(Flag::carry) : reset_flag(Flag::carry);

    pc++;
}

void CPU::rlca() noexcept {
    uint8_t old_a = af.bytes.high;
    uint8_t msb = (old_a & 0x80) >> 7;
    
    af.bytes.high = (af.bytes.high << 1) | msb;

    reset_flag(Flag::zero);
    reset_flag(Flag::subtraction);
    reset_flag(Flag::half_carry);
    msb ? set_flag(Flag::carry) : reset_flag(Flag::carry);

    pc++;
}

void CPU::rra() noexcept {
    uint8_t lsb = af.bytes.high & 0x01;
    uint8_t old_carry = get_flag(Flag::carry) ? 1 : 0;

    af.bytes.high = (af.bytes.high >> 1) | (old_carry << 7);

    reset_flag(Flag::zero);
    reset_flag(Flag::subtraction);
    reset_flag(Flag::half_carry);
    lsb ? set_flag(Flag::carry) : reset_flag(Flag::carry);

    pc++;
}

void CPU::rrca() noexcept {
    uint8_t old_a = af.bytes.high;
    uint8_t lsb = (old_a & 0x01);
    
    af.bytes.high = (af.bytes.high >> 1) | (lsb << 7);

    reset_flag(Flag::zero);
    reset_flag(Flag::subtraction);
    reset_flag(Flag::half_carry);
    lsb ? set_flag(Flag::carry) : reset_flag(Flag::carry);

    pc++;
}

void CPU::rst(uint8_t vector) noexcept {
    push(pc + 1);
    pc = vector * 8;
}

void CPU::sbc(uint8_t value) noexcept {
    uint8_t old_a = af.bytes.high;
    uint8_t old_carry = get_flag(Flag::carry) ? 1 : 0;

    af.bytes.high = old_a - (value + old_carry);

    af.bytes.high == 0 ? set_flag(Flag::zero) : reset_flag(Flag::zero);
    set_flag(Flag::subtraction);

    (old_a & 0x0f) < ((value & 0x0f) + old_carry) ? set_flag(Flag::half_carry) : reset_flag(Flag::half_carry);
    old_a < (value + old_carry) ? set_flag(Flag::carry) : reset_flag(Flag::carry);

    pc++;
}

void CPU::scf() noexcept {
    reset_flag(Flag::subtraction);
    reset_flag(Flag::half_carry);
    set_flag(Flag::carry);

    pc++;
}

void CPU::sub(uint8_t value) noexcept {
    uint8_t old_a = af.bytes.high;
    af.bytes.high = old_a - value;

    af.bytes.high == 0 ? set_flag(Flag::zero) : reset_flag(Flag::zero);
    set_flag(Flag::subtraction);
    (old_a & 0x0f) < (value & 0x0f) ? set_flag(Flag::half_carry) : reset_flag(Flag::half_carry);
    old_a < value ? set_flag(Flag::carry) : reset_flag(Flag::carry);

    pc++;
}

void CPU::rl(uint8_t &reg8) noexcept {
    uint8_t old_carry = get_flag(Flag::carry) ? 1 : 0;

    (reg8 & 0x80) ? set_flag(Flag::carry) : reset_flag(Flag::carry);

    reg8 = (reg8 << 1) | old_carry;

    reg8 == 0 ? set_flag(Flag::zero) : reset_flag(Flag::zero);
    reset_flag(Flag::subtraction);
    reset_flag(Flag::half_carry);
}

void CPU::rlc(uint8_t &reg8) noexcept {
    uint8_t msb = (reg8 & 0x80) >> 7;
    
    reg8 = (reg8 << 1) | msb;

    reg8 == 0 ? set_flag(Flag::zero) : reset_flag(Flag::zero);
    reset_flag(Flag::subtraction);
    reset_flag(Flag::half_carry);
    msb ? set_flag(Flag::carry) : reset_flag(Flag::carry);
}

void CPU::rr(uint8_t &reg8) noexcept {
    uint8_t old_carry = get_flag(Flag::carry) ? 1 : 0;

    (reg8 & 0x01) ? set_flag(Flag::carry) : reset_flag(Flag::carry);

    reg8 = (reg8 >> 1) | (old_carry << 7);

    reg8 == 0 ? set_flag(Flag::zero) : reset_flag(Flag::zero);
    reset_flag(Flag::subtraction);
    reset_flag(Flag::half_carry);
}

void CPU::rrc(uint8_t &reg8) noexcept {
    uint8_t lsb = reg8 & 0x01;
    
    reg8 = (reg8 >> 1) | (lsb << 7);

    reg8 == 0 ? set_flag(Flag::zero) : reset_flag(Flag::zero);
    reset_flag(Flag::subtraction);
    reset_flag(Flag::half_carry);
    lsb ? set_flag(Flag::carry) : reset_flag(Flag::carry);
}

void CPU::sla(uint8_t &reg8) noexcept {
    uint8_t msb = reg8 & 0x80;

    reg8 <<= 1;

    reg8 == 0 ? set_flag(Flag::zero) : reset_flag(Flag::zero);
    reset_flag(Flag::subtraction);
    reset_flag(Flag::half_carry);
    msb ? set_flag(Flag::carry) : reset_flag(Flag::carry);
}

void CPU::sra(uint8_t &reg8) noexcept {
    uint8_t lsb = reg8 & 0x01;
    uint8_t msb = reg8 & 0x80;
    
    reg8 = (reg8 >> 1) | msb;

    reg8 == 0 ? set_flag(Flag::zero) : reset_flag(Flag::zero);
    reset_flag(Flag::subtraction);
    reset_flag(Flag::half_carry);
    lsb ? set_flag(Flag::carry) : reset_flag(Flag::carry);
}

void CPU::srl(uint8_t &value) noexcept {
    (value & 0x01) ? set_flag(Flag::carry) : reset_flag(Flag::carry);

    value = (value >> 1);
    
    value == 0 ? set_flag(Flag::zero) : reset_flag(Flag::zero);
    reset_flag(Flag::subtraction);
    reset_flag(Flag::half_carry);
}

void CPU::swap(uint8_t &reg8) noexcept {
    uint8_t lower = reg8 & 0x0f;
    uint8_t upper = reg8 & 0xf0;
    uint8_t swapped_value = (lower << 4) | (upper >> 4);
    reg8 = swapped_value;

    swapped_value == 0 ? set_flag(Flag::zero) : reset_flag(Flag::zero);
    reset_flag(Flag::subtraction);
    reset_flag(Flag::half_carry);
    reset_flag(Flag::carry);
}


void CPU::set_flag(Flag flag) noexcept { af.bytes.low |= static_cast<uint8_t>(flag); }
void CPU::reset_flag(Flag flag) noexcept { af.bytes.low &= ~static_cast<uint8_t>(flag); }
bool CPU::get_flag(Flag flag) const noexcept { return (af.bytes.low & static_cast<uint8_t>(flag)) != 0; }

void CPU::update_clock_cycles(uint8_t cycle) noexcept {
    total_cycle += cycle;
}

void CPU::print_debug() noexcept {
    std::cout  
          << "PC: 0x" << std::setfill('0') << std::setw(4) << std::hex << pc 
          << " | SP: 0x" << std::setfill('0') << std::setw(4) << sp 
          << " | Opcode: 0x" << std::setfill('0') << std::setw(2) << static_cast<int>(mmu.read(pc))
          << " | A: 0x" << std::setfill('0') << std::setw(2) << static_cast<int>(af.bytes.high) 
          << " | BC: 0x" << std::setfill('0') << std::setw(4) << bc.word
          << " | DE: 0x" << std::setfill('0') << std::setw(4) << de.word
          << " | HL: 0x" << std::setfill('0') << std::setw(4) << hl.word
          << " | Cycles: " << std::dec << total_cycle
          << " - Flags: ["
          << ((get_flag(Flag::zero))        ? 'Z' : '-')
          << ((get_flag(Flag::subtraction)) ? 'N' : '-')
          << ((get_flag(Flag::half_carry))  ? 'H' : '-')
          << ((get_flag(Flag::carry))       ? 'C' : '-')
          << "]" << std::endl; 
}