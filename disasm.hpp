#ifndef DISASM_HPP
#define DISASM_HPP

#include <string>
#include "common.h"
#include "memory.hpp"
#include "cpu.hpp"

class Disasm {

public:
    Disasm(Memory& mem, CPU &cpu);
    std::string mem(WORD addr);
    void unasm(WORD addr, std::string &assembler, std::string &ea);
    void save_pr();

private:
    Memory &memory;
    CPU &cpu;

    WORD pr[4];

    void disasm(BYTE opcode, std::string &assembler);
    void disasm(WORD addr, BYTE opcode, SBYTE operand, std::string &assembler, std::string &ea);
    WORD disasm_ea(WORD addr, int addressing, SBYTE disp);
    std::string operandPR(BYTE opcode);
    std::string operand_str(SBYTE operand, int base);
    std::string operand_addressing(int addressing, SBYTE operand);
    std::string ea_jump(WORD addr, int addressing, SBYTE operand);
    std::string ea_memory(WORD addr, int addressing, SBYTE operand);
};

#endif