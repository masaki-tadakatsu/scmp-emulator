#include "common.h" 
#include "memory.hpp"
#include "cpu.hpp" 


CPU::CPU(Memory &mem): memory(mem)
{
    CPU::reset();
    CPU::run_mode(RUN);
}

void CPU::run_mode(CPUMODE mode)
{
    runmode = mode;
}

void CPU::reset()
{
    // clear all registers
    reg.AC = 0;
    reg.ER = 0;
    reg.SR = 0;
    reg.PR[0] = 0;
    reg.PR[1] = 0;
    reg.PR[2] = 0;
    reg.PR[3] = 0;
}

CPUSTAT CPU::clock()
{
    CPUSTAT stat;

    stat = CPU::interrupt();                   // if IE & SA then interrupt
    if (stat == SUCCESS){
        BYTE opcode = CPU::fetch();
        if ((opcode & BIT_SIGN_BYTE) == 0){
            stat = CPU::exec(opcode);
        }
        else {
            SBYTE disp = CPU::fetch();      // fetch 2nd byte of instruction
            stat = CPU::exec(opcode, disp);
        }
    }
    return (stat);
}

CPUSTAT CPU::interrupt()
{
    if ((reg.SR & BIT_SR_IE) != 0 && ((reg.SR & BIT_SR_SA) != 0)){
        reg.SR &= ~BIT_SR_IE;       // clear IE
        CPU::execXPPC(3);           // XPPC P3

        return (INTERRPT);   
    }
    return (SUCCESS);
}
BYTE CPU::fetch()
{
    WORD ea = calc_ea(0, 1);            // increment PC
    reg.PR[0] = ea;
    BYTE data = memory.read(ea);        // memory fetch

    return (data);
}

WORD CPU::calc_ea(int pr, SBYTE disp)
{
    WORD ea = (reg.PR[pr] & BIT_PR_PAGE) | ((reg.PR[pr] + disp) & ~BIT_PR_PAGE);

    return (ea);
}

WORD CPU::get_ea(int addressing, SBYTE disp)
{
    WORD ea;

    if (disp == -128){
        disp = reg.ER;
    }

    int pr = addressing & BIT_OPCODE_PR;

    if ((addressing & BIT_OPCODE_MODE) == 0){      // Indexed Addressing
        ea = calc_ea(pr, disp);
    }
    else {                                  // Auto-Indexed Addressing
        if (disp < 0){
            ea = calc_ea(pr, disp);
            reg.PR[pr] = ea; 
        }
        else {
            ea = reg.PR[pr];
            reg.PR[pr] = calc_ea(pr, disp);
        }
    }

    return (ea);
}

SBYTE CPU::get_data(int addressing, SBYTE disp)
{
    BYTE data;
    if ((addressing & (BIT_OPCODE_MODE | BIT_OPCODE_PR)) == 4){   // Immediate Addressing
        data = (BYTE)disp;
    }
    else {                              // Indexed or Auto-Indexed Addressing
        WORD ea = get_ea(addressing, disp);
        data = memory.read(ea);
    }

    return (data);
}

BYTE CPU::add_byte(BYTE a, BYTE b)
{
    WORD c = (WORD)a + (WORD)b + ((reg.SR & BIT_SR_CY) == 0 ? 0: 1);

    // Carry
    reg.SR = (reg.SR & ~BIT_SR_CY) | ((c & 0x0100) >> 1);

    // オーバーフローの条件　正+正=負、負+負=正
    reg.SR &= ~BIT_SR_OV;
    if (((a & BIT_SIGN_BYTE) == (b & BIT_SIGN_BYTE)) && ((a & BIT_SIGN_BYTE) != (c & BIT_SIGN_BYTE))){
        reg.SR = reg.SR | BIT_SR_OV;
    }

    return ((BYTE)c);
}

BYTE CPU::add_bcd(BYTE a, BYTE b)
{
    WORD c = (WORD)a + (WORD)b + ((reg.SR & BIT_SR_CY) == 0 ? 0: 1);

    if (c % 16 >= 0x0a){
        c += 6;
    }

    if (c >= 0xa0){
        c += 0x60;
    }

    // Carry
    reg.SR = (reg.SR & ~BIT_SR_CY) | ((c & 0x0100) >> 1);

    return ((BYTE)c);
}
