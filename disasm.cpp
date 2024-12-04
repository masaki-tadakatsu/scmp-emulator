#include <iostream>
#include <iomanip>      // for std::setw, std::setfill>
#include <sstream>
#include <string>
#include "common.h" 
#include "util.hpp"
#include "memory.hpp"
#include "cpu.hpp"
#include "monitor.hpp"
#include "disasm.hpp"


Disasm::Disasm(Memory &mem, CPU &cpu): memory(mem), cpu(cpu)
{
}

void Disasm::unasm(WORD addr, std::string &assembler, std::string &ea)
{
    BYTE opcode = memory.read(addr);

    if ((opcode & BIT_SIGN_BYTE) == 0){
        disasm(opcode, assembler);
        ea = "";
    }
    else {
        SBYTE operand = memory.read(addr + 1);
        disasm(addr, opcode, operand, assembler, ea);
    }
}

std::string Disasm::mem(WORD addr)
{
    std::stringstream out;
    BYTE data;

    out << std::hex;
    out << Util::hex2str(addr);
    out << ":";

    data = memory.read((WORD)addr);
    out << Util::hex2str(data);
    if ((data & BIT_SIGN_BYTE) != 0){
        out << " ";
        data = memory.read(addr + 1);
        out << Util::hex2str(data);
    }

    return (out.str());
}

void Disasm::disasm(BYTE opcode, std::string &assembler)
{
    std::string out;
    int inst;

    inst = opcode;
    if (OPE_XPAL <= opcode && opcode <= OPE_XPPC + 3){
        inst &= ~BIT_OPCODE_PR;
    }

    switch (inst){
    case OPE_HALT:  // Halt
        assembler = "HALT";
        break;

    case OPE_XAE:   // Exchange AC and Extention   AC <-> E
        assembler = "XAE";
        break;

    case OPE_CCL:   // Clear Carry/Link
        assembler = "CCL";
        break;

    case OPE_SCL:   // Set Carry/Link
        assembler = "SCL";
        break;

    case OPE_DINT:  // Disable Interrupt
        assembler = "DINT";
        break;

    case OPE_IEN:   // Enable Interrupt
        assembler = "IEN";
        break;

    case OPE_CSA:   // Copy Status to AC
        assembler = "CSA";
        break;

    case OPE_CAS:   // Copy AC to Status
        assembler = "CAS";
        break;

    case OPE_NOP:   // No Operation
        assembler = "NOP";
        break;

    case OPE_SIO:   // Serial Input/Output
        assembler = "SIO";
        break;

    case OPE_SR:    // Shift Right
        assembler = "SR";
        break;

    case OPE_SRL:   // Shift Right with Link
        assembler = "SRL";
        break;

    case OPE_RR:    // Rotate Right
        assembler = "RR";
        break;

    case OPE_RRL:   // Rotate Right with Link
        assembler = "RRL";
        break;

    case OPE_XPAL:  // Exchange Pointer Low
        assembler = "XPAL " + operandPR(opcode);
        break;

    case OPE_XPAH:  // Exchange Pointer High
        assembler = "XPAH " + operandPR(opcode);
        break;

    case OPE_XPPC:  // Exchange Pointer with PC
        assembler = "XPPC " + operandPR(opcode);
        break;

    case OPE_LDE:   // Load AC from Extention
        assembler = "LDE";
        break;

    case OPE_ANE:   // AND Extention
        assembler = "ANE";
        break;

    case OPE_ORE:   // OR Extention
        assembler = "ORE";
        break;

    case OPE_XRE:   // Exclusive OR Extention
        assembler = "XRE";
        break;

    case OPE_DAE:   // Decimal Add Extention
        assembler = "DAE";
        break;

    case OPE_ADE:   // Add Extention
        assembler = "ADE";
        break;

    case OPE_CAE:   // Compulement and Add Extention
        assembler = "CAE";
        break;

    case OPE_PUTC:  // putc() for NIBL
        assembler = "PUTC";
        break;

    case OPE_GETC:  // getc() for NIBL
        assembler = "GETC";
        break;

    default:
        assembler = "UND";
    }
}

std::string Disasm::operandPR(BYTE opcode)
{
    std::string out[] = {"PC", "P1", "P2", "P3"};

    return (out[opcode & BIT_OPCODE_PR]);
}

void Disasm::disasm(WORD addr, BYTE opcode, SBYTE operand, std::string &assembler, std::string &ea)
{
    int inst = opcode;

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

    ea = "";
    switch (inst){
    case OPE_DLY:
        assembler = "DLY  " + operand_str(operand, 10);
        break;

    case OPE_JMP:
        assembler = "JMP  " + operand_addressing(opcode & BIT_OPCODE_PR, operand);
        ea = ea_jump(addr, opcode & BIT_OPCODE_PR, operand);
        break;

    case OPE_JP:
        assembler = "JP   " + operand_addressing(opcode & BIT_OPCODE_PR, operand);
        ea = ea_jump(addr, opcode & BIT_OPCODE_PR, operand);
        break;

    case OPE_JZ:
        assembler = "JZ   " + operand_addressing(opcode & BIT_OPCODE_PR, operand);
        ea = ea_jump(addr, opcode & BIT_OPCODE_PR, operand);
        break;

    case OPE_JNZ:
        assembler = "JNZ  " + operand_addressing(opcode & BIT_OPCODE_PR, operand);
        ea = ea_jump(addr, opcode & BIT_OPCODE_PR, operand);
        break;

    case OPE_ILD:
        assembler = "ILD  " + operand_addressing(opcode & BIT_OPCODE_PR, operand);
        ea = ea_memory(addr, opcode & BIT_OPCODE_PR, operand);
        break;

    case OPE_DLD:
        assembler = "DLD  " + operand_addressing(opcode & BIT_OPCODE_PR, operand);
        ea = ea_memory(addr, opcode & BIT_OPCODE_PR, operand);
        break;

    case OPE_LD:    // LD, LDI
        if (opcode == OPE_LDI){
            assembler = "LDI  ";
            assembler += operand_addressing(opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR), operand);
        }
        else {
            assembler = "LD   ";
            assembler += operand_addressing(opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR), operand);
            ea = ea_memory(addr, opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR), operand);
        }
        break;

    case OPE_ST:
        if ((opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR)) == 4){
            assembler = "UND";
        }
        else {
            assembler = "ST   ";
            assembler += operand_addressing(opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR), operand);
            ea = ea_memory(addr, opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR), operand);
        }
        break;

    case OPE_AND:   // AND, ANI
        if (opcode == OPE_ANI){
            assembler = "ANI  ";
            assembler += operand_addressing(opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR), operand);
        }
        else {
            assembler = "AND  ";
            assembler += operand_addressing(opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR), operand);
            ea = ea_memory(addr, opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR), operand);
        }
        break;

    case OPE_OR:    // OR, ORI
        if (opcode == OPE_ORI){
            assembler = "ORI  ";
            assembler += operand_addressing(opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR), operand);
        }
        else {
            assembler = "OR   ";
            assembler += operand_addressing(opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR), operand);
            ea = ea_memory(addr, opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR), operand);
        }
        break;

    case OPE_XOR:   // XOR, XRI
        if (opcode == OPE_XRI){
            assembler = "XRI  ";
            assembler += operand_addressing(opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR), operand);
        }
        else {
            assembler = "XOR  ";
            assembler += operand_addressing(opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR), operand);
            ea = ea_memory(addr, opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR), operand);
        }
        break;

    case OPE_DAD:   // DAD, DAI
        if (opcode == OPE_DAI){
            assembler = "DAI  ";
            assembler += operand_addressing(opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR), operand);
        }
        else {
            assembler = "DAD  ";
            assembler += operand_addressing(opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR), operand);
            ea = ea_memory(addr, opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR), operand);
        }
        break;

    case OPE_ADD:   // ADD, ADI
        if (opcode == OPE_ADI){
            assembler = "ADI  ";
            assembler += operand_addressing(opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR), operand);
        }
        else {
            assembler = "ADD  ";
            assembler += operand_addressing(opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR), operand);
            ea = ea_memory(addr, opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR), operand);
        }
        break;

    case OPE_CAD:   // CAD, CAI
        if (opcode == OPE_CAI){
            assembler = "CAI  ";
            assembler += operand_addressing(opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR), operand);
        }
        else {
            assembler = "CAD  ";
            assembler += operand_addressing(opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR), operand);
            ea = ea_memory(addr, opcode & (BIT_OPCODE_MODE | BIT_OPCODE_PR), operand);
        }
        break;

    default:        // undefined instruction
        assembler = "UND";
    }
}

std::string Disasm::operand_str(SBYTE operand, int base)
{
    std::stringstream out;

    if (base == 10){
        out << std::dec << (int)operand;
    }
    else {
        out << "0x" << Util::hex2str((BYTE)operand);
    }

    return (out.str());
}

std::string Disasm::operand_addressing(int addressing, SBYTE operand)
{
    std::string mode[] = {"", "(P1)", "(P2)", "(P3)"};
    std::string out;

    if ((addressing & (BIT_OPCODE_MODE | BIT_OPCODE_PR)) == 4){
        out = operand_str(operand, 16);
    }
    else {
        if ((addressing & BIT_OPCODE_MODE) != 0){
            out = "@";
        }
        out += operand_str(operand, 10) + mode[addressing & BIT_OPCODE_PR];

    }
    return (out);
}

std::string Disasm::ea_jump(WORD addr, int addressing, SBYTE disp)
{
    std::stringstream out;

    WORD ea = disasm_ea(addr, addressing, disp);

    out << "JUMP=" << Util::hex2str(ea);

    return (out.str());
}

std::string Disasm::ea_memory(WORD addr, int addressing, SBYTE operand)
{
    std::stringstream out;    
    WORD ea = disasm_ea(addr, addressing, operand); 

    out << "EA=" << Util::hex2str(ea);
    out << "(" << Util::hex2str(memory.read(ea)) << ")"; 

    return (out.str());
}

void Disasm::save_pr()
{
    pr[0] = cpu.getPC() + 1;
    pr[1] = cpu.getP1();
    pr[2] = cpu.getP2();
    pr[3] = cpu.getP3();
}

WORD Disasm::disasm_ea(WORD addr, int addressing, SBYTE disp)
{
    WORD ptr, ea;

    ptr = pr[addressing & BIT_OPCODE_PR];

    if (disp == -128){
        disp = cpu.getER();
    }
    if ((addressing & BIT_OPCODE_MODE) == 0){      // Indexed Addressing
        ea = (ptr & BIT_PR_PAGE) | ((ptr + disp) & ~BIT_PR_PAGE);
    }
    else {                                  // Auto-Indexed Addressing
        if (disp < 0){
            ea = (ptr & BIT_PR_PAGE) | ((ptr + disp + 1) & ~BIT_PR_PAGE);
        }
        else {
            ea = ptr;       
        }
    }
    return (ea);
}