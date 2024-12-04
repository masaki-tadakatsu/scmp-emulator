#include <unistd.h>     // for sleep()
#include "common.h" 
#include "memory.hpp" 
#include "cpu.hpp" 

CPUSTAT CPU::exec(BYTE opcode, SBYTE disp)
{
    CPUSTAT stat;
    int inst;

    inst = opcode;

    if (0x90 <= opcode && opcode <= 0x9f){  // JMP, JP, JZ, JNZ
        inst &= 0xfc;
    }
    else if (0xA8 <= opcode && opcode <= 0xAB){  // ILD
        inst &= 0xfc;
    }
    else if (0xB8 <= opcode && opcode <= 0xBB){  // ILD
        inst &= 0xfc;
    }
    else if (opcode >= 0xc0){ // LD, ST, AND, OR, XOR, DAD, ADD, CAD
        inst &= 0xf8;
    }
    switch (inst){
    case OPE_DLY:
        stat = CPU::execDLY(opcode, disp);  // Delay
        break;

    case OPE_JMP:
        stat = CPU::execJMP(opcode, disp);  // Jump
        break;

    case OPE_JP:
        stat = CPU::execJP(opcode, disp);   // Jump if Positive
        break;

    case OPE_JZ:
        stat = CPU::execJZ(opcode, disp);   // Jump if Zero
        break;

    case OPE_JNZ:
        stat = CPU::execJNZ(opcode, disp);  // Jump if Not Zero
        break;

    case OPE_ILD:
        stat = CPU::execILD(opcode, disp);  // Increment and Load
        break;

    case OPE_DLD:
        stat = CPU::execDLD(opcode, disp);  // Decriment and Load
        break;

    case OPE_LD:    // LD, LDI
        stat = CPU::execLD(opcode, disp);   // Load
        break;

    case OPE_ST:
        stat = CPU::execST(opcode, disp);   // Store
        break;

    case OPE_AND:   // AND, ANI
        stat = CPU::execAND(opcode, disp);  // AND
        break;

    case OPE_OR:    // OR, ORI
        stat = CPU::execOR(opcode, disp);   // OR
        break;

    case OPE_XOR:   // XOR, XRI
        stat = CPU::execXOR(opcode, disp);  // Exclusive OR
        break;

    case OPE_DAD:   // DAD, DAI
        stat = CPU::execDAD(opcode, disp);  // Decimal Add
        break;

    case OPE_ADD:   // ADD, ADI
        stat = CPU::execADD(opcode, disp);  // Add
        break;

    case OPE_CAD:   // CAD, CAI
        stat = CPU::execCAD(opcode, disp);  // Complement and Add
        break;

    default:
        stat = UNDEFINED;   // undefined instruction
    }

    return (stat);
}

CPUSTAT CPU::execDLY(BYTE opcode, SBYTE disp)   // Delay
{
     // micro cycles  1 microcycles
    int  delay = 13 + 2 * (UINT32)reg.AC + 2 * (UINT32)disp + ((UINT32)disp << 9); // micro cycles

    sleep((double)delay / (1000 * 1000));    // 1000 micro-cycles = 1 micro Second (Clock=4MHz)

    // 命令終了時のACの値が不明

    return (SUCCESS);
}

CPUSTAT CPU::execJMP(BYTE opcode, SBYTE disp)   // Jump
{
    reg.PR[0] = calc_ea(opcode & BIT_OPCODE_PR, disp);

    return (SUCCESS);
}

CPUSTAT CPU::execJP(BYTE opcode, SBYTE disp)   // Jump if Positive
{
    if ((reg.AC & BIT_SIGN_BYTE) == 0){
        reg.PR[0] = calc_ea(opcode & BIT_OPCODE_PR, disp);
    }
    return (SUCCESS);
}

CPUSTAT CPU::execJZ(BYTE opcode, SBYTE disp)   // Jump if Zero
{
    if (reg.AC == 0){
        reg.PR[0] = calc_ea(opcode & BIT_OPCODE_PR, disp);
    }
    return (SUCCESS);
}

CPUSTAT CPU::execJNZ(BYTE opcode, SBYTE disp)  // Jump if Not Zero
{
    if (reg.AC != 0){
        reg.PR[0] = calc_ea(opcode & BIT_OPCODE_PR, disp);
    }
    return (SUCCESS);
}

CPUSTAT CPU::execILD(BYTE opcode, SBYTE disp)   // Increment and Load
{
    WORD ea = calc_ea(opcode & BIT_OPCODE_PR, disp);
    BYTE data = memory.read(ea) + 1;
    memory.write(ea, data);
    reg.AC = data;

    return (SUCCESS);
}

CPUSTAT CPU::execDLD(BYTE opcode, SBYTE disp)   // Decriment and Load
{
    WORD ea = calc_ea(opcode & BIT_OPCODE_PR, disp);
    BYTE data = memory.read(ea) - 1;
    memory.write(ea, data);
    reg.AC = data;

    return (SUCCESS);
}

CPUSTAT CPU::execLD(BYTE opcode, SBYTE disp)    // Load
{
    reg.AC = get_data(opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR), disp);

    return (SUCCESS);
}

CPUSTAT CPU::execST(BYTE opcode, SBYTE disp)    // Store
{
    if ((opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR)) == 4){          // Immediate Addressing
        return (UNDEFINED);
    }

    WORD ea = get_ea(opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR), disp);
    memory.write(ea, reg.AC);

    return (SUCCESS);
}

CPUSTAT CPU::execAND(BYTE opcode, SBYTE disp)   // AND
{
    reg.AC &= get_data(opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR), disp);

    return (SUCCESS);
}

CPUSTAT CPU::execOR(BYTE opcode, SBYTE disp)    // OR
{
    reg.AC |= get_data(opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR), disp);

    return (SUCCESS);
}

CPUSTAT CPU::execXOR(BYTE opcode, SBYTE disp)   // Exclusive OR
{
    reg.AC ^= get_data(opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR), disp);

    return (SUCCESS);
}

CPUSTAT CPU::execDAD(BYTE opcode, SBYTE disp)   // Decimal Add
{
    BYTE data = get_data(opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR), disp);
    reg.AC = add_bcd(reg.AC, data);

    return (SUCCESS);
}

CPUSTAT CPU::execADD(BYTE opcode, SBYTE disp)   // Add
{
    BYTE data = get_data(opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR), disp);
    reg.AC = add_byte(reg.AC, data);

    return (SUCCESS);
}

CPUSTAT CPU::execCAD(BYTE opcode, SBYTE disp)   // Complement and Add
{
    BYTE data = get_data(opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR), disp);
    reg.AC = add_byte(reg.AC, ~data);

    return (SUCCESS);
}
