#include <iostream>
#include <iomanip>      // for std::setw, std::setfill>
#include "common.h"
#include "util.hpp"
#include "memory.hpp" 
#include "cpu.hpp" 

CPUSTAT CPU::exec(BYTE opcode)
{
    CPUSTAT stat;
    int inst;

    inst = opcode;
    if (OPE_XPAL <= opcode && opcode <= OPE_XPPC + 3){
        inst &= ~BIT_OPCODE_PR;
    }

    switch (inst){
    case OPE_HALT:  // Halt
        stat = CPU::execHALT(opcode);
        break;

    case OPE_XAE:   // Exchange AC and Extention   AC <-> E
        stat = CPU::execXAE(opcode);
        break;

    case OPE_CCL:   // Clear Carry/Link
        stat = CPU::execCCL(opcode);
        break;

    case OPE_SCL:   // Set Carry/Link
        stat = CPU::execSCL(opcode);
        break;

    case OPE_DINT:  // Disable Interrupt
        stat = CPU::execDINT(opcode);
        break;

    case OPE_IEN:   // Enable Interrupt
        stat = CPU::execIEN(opcode);
        break;

    case OPE_CSA:   // Copy Status to AC
        stat = CPU::execCSA(opcode);
        break;

    case OPE_CAS:   // Copy AC to Status
        stat = CPU::execCAS(opcode);
        break;

    case OPE_NOP:   // No Operation
        stat = CPU::execNOP(opcode);
        break;

    case OPE_SIO:   // Serial Input/Output
        stat = CPU::execSIO(opcode);
        break;

    case OPE_SR:    // Shift Right
        stat = CPU::execSR(opcode);
        break;

    case OPE_SRL:   // Shift Right with Link
        stat = CPU::execSRL(opcode);
        break;

    case OPE_RR:    // Rotate Right
        stat = CPU::execRR(opcode);
        break;

    case OPE_RRL:   // Rotate Right with Link
        stat = CPU::execRRL(opcode);
        break;

    case OPE_XPAL:  // Exchange Pointer Low
        stat = CPU::execXPAL(opcode);
        break;

    case OPE_XPAH:  // Exchange Pointer High
        stat = CPU::execXPAH(opcode);
        break;

    case OPE_XPPC:  // Exchange Pointer with PC
        stat = CPU::execXPPC(opcode);
        break;

    case OPE_LDE:   // Load AC from Extention
        stat = CPU::execLDE(opcode);
        break;

    case OPE_ANE:   // AND Extention
        stat = CPU::execANE(opcode);
        break;

    case OPE_ORE:   // OR Extention
        stat = CPU::execORE(opcode);
        break;

    case OPE_XRE:   // Exclusive OR Extention
        stat = CPU::execXRE(opcode);
        break;

    case OPE_DAE:   // Decimal Add Extention
        stat = CPU::execDAE(opcode);
        break;

    case OPE_ADE:   // Add Extention
        stat = CPU::execADE(opcode);
        break;

    case OPE_CAE:   // Compulement and Add Extention
        stat = CPU::execCAE(opcode);
        break;

    case OPE_PUTC:   // PUTC() for NIBL
        stat = CPU::execPUTC(opcode);
        break;

    case OPE_GETC:   // GETC() for NIBL
        stat = CPU::execGETC(opcode);
        break;

    default:
        stat = UNDEFINED;
    }

    return (stat);
}

//
// 1byte命令の実行部
//

CPUSTAT CPU::execHALT(BYTE opcode)  // Halt
{
    return (HALT);
}

CPUSTAT CPU::execXAE(BYTE opcode)   // Exchange AC and Extention
{
    BYTE tmp;

    tmp = reg.AC;
    reg.AC = reg.ER;
    reg.ER = tmp;

    return (SUCCESS);
}

CPUSTAT CPU::execCCL(BYTE opcode)   // Clear Carry/Link

{
    reg.SR &= ~BIT_SR_CY;

    return (SUCCESS);
}

CPUSTAT CPU::execSCL(BYTE opcode)   // Set Carry/Link
{
    reg.SR |= BIT_SR_CY;

    return (SUCCESS);
}

CPUSTAT CPU::execDINT(BYTE opcode)  // Disable Interrupt
{
    reg.SR &= ~BIT_SR_IE;

    return (SUCCESS);
}

CPUSTAT CPU::execIEN(BYTE opcode)   // Enable Interrupt
{
    reg.SR |= BIT_SR_IE;

    return (SUCCESS);
}

CPUSTAT CPU::execCSA(BYTE opcode)   // Copy Status to AC
{
    reg.AC = reg.SR;

    return (SUCCESS);
}

CPUSTAT CPU::execCAS(BYTE opcode)    // Copy AC to Status
{
    reg.SR = (reg.SR & (BIT_SR_SA | BIT_SR_SB)) | (reg.AC & ~(BIT_SR_SA | BIT_SR_SB));
    
    return (SUCCESS);
}

CPUSTAT CPU::execNOP(BYTE opcode)    // No Operation
{
    return (SUCCESS);
}

CPUSTAT CPU::execSIO(BYTE opcode)    // Serial Input/Output
{
    reg.ER >>= 1;

    return (SUCCESS);
}

CPUSTAT CPU::execSR(BYTE opcode)   // Shift Right
{
    reg.AC >>= 1;

    return (SUCCESS);
}

CPUSTAT CPU::execSRL(BYTE opcode)   // Shift Right with Link
{
    reg.AC >>= 1;
    reg.AC |= (reg.SR & BIT_SR_CY);

    return (SUCCESS);
}

CPUSTAT CPU::execRR(BYTE opcode)    // Rotate Right
{
    reg.AC = (reg.AC >> 1) | ((reg.AC & 1) << 7);

    return (SUCCESS);
}

CPUSTAT CPU::execRRL(BYTE opcode)   // Rotate Right with Link
{
    BYTE lsb = reg.AC & 1;
    reg.AC = (reg.AC >> 1) | (reg.SR & BIT_SR_CY);
    reg.SR = (reg.SR & ~BIT_SR_CY) | (lsb << 7);

    return (SUCCESS);
}

CPUSTAT CPU::execXPAL(BYTE opcode)  // Exchange Pointer Low
{
    int pr = opcode & 3;

    int tmp = reg.AC;
    reg.AC = reg.PR[pr] & 0x00ff;
    reg.PR[pr] = (reg.PR[pr] & 0xff00) | tmp;

    return (SUCCESS);
}

CPUSTAT CPU::execXPAH(BYTE opcode)  // Exchange Pointer High
{
    int pr = opcode & 3;

    int tmp = reg.AC << 8;
    reg.AC = reg.PR[pr] >> 8;
    reg.PR[pr] = (reg.PR[pr] & 0x00ff) | tmp;

    return (SUCCESS);
}

CPUSTAT CPU::execXPPC(BYTE opcode)  // Exchange Pointer with PC
{
    int pr = opcode & 3;

    WORD tmp = reg.PR[0];
    reg.PR[0] = reg.PR[pr];
    reg.PR[pr] = tmp;

    return (SUCCESS);
}

CPUSTAT CPU::execLDE(BYTE opcode)   // Load AC from Extention
{
    reg.AC = reg.ER;

    return (SUCCESS);
}

CPUSTAT CPU::execANE(BYTE opcode)   // AND Extention
{
    reg.AC &= reg.ER;

    return (SUCCESS);
}

CPUSTAT CPU::execORE(BYTE opcode)   // OR Extention
{
    reg.AC |= reg.ER;

    return (SUCCESS);
}

CPUSTAT CPU::execXRE(BYTE opcode)   // Exclusive OR Extention
{
    reg.AC ^= reg.ER;

    return (SUCCESS);
}

CPUSTAT CPU::execDAE(BYTE opcode)   // Decimal Add Extention
{
    reg.AC = add_bcd(reg.AC, reg.ER);

    return (SUCCESS);
}

CPUSTAT CPU::execADE(BYTE opcode)   // Add Extention
{
    reg.AC = add_byte(reg.AC, reg.ER);

    return (SUCCESS);
}

CPUSTAT CPU::execCAE(BYTE opcode)   // Compulement and Add Extention
{
    reg.AC = add_byte(reg.AC, ~reg.ER);

    return (SUCCESS);
}

CPUSTAT CPU::execPUTC(BYTE opcode)   // Put Character for NIBL
{
    if (runmode == RUN){
        putchar((char)reg.AC & 0x7f);
    }
    else {
        std::cout << "\nPUTC(0x" << Util::hex2str(reg.AC) << ")" << ":" << reg.AC << std::endl << std::endl;
    }
    return (SUCCESS);
}

CPUSTAT CPU::execGETC(BYTE opcode)   // Get Character for NIBL
{
    if (runmode != RUN){
        std::cout << "\nGETC()" << ":";
    }

	char c = getchar();
	
    c = std::toupper(c);

	if (c == 0x0a){     // LF--> CR
        c = 0x0d; 
    }

	reg.AC = reg.ER = c;

    return (SUCCESS);
}

