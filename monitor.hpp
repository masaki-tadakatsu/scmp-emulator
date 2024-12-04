#ifndef MONITOR_HPP
#define MONITOR_HPP

#include <iostream>
#include <sstream>
#include <string>
#include "common.h"
#include "memory.hpp"
#include "cpu.hpp"
#include "disasm.hpp"

#define PREEXEC 1

enum RESULT {
    OK,
    NG
};

enum BP_STAT {
    BP_NONE,
    BP_ENABLE,
    BP_DISABLE
};

class Monitor {

public:
    Monitor(Memory& mem, CPU &cpu, Disasm &disasm);
    void monitor();

private:
    Memory &memory;
    CPU &cpu;
    Disasm &disasm;

    WORD BPaddr;        // Break Point memory address
    BP_STAT BPstat;     // Break Point status

    RESULT help(std::stringstream &line);
    RESULT dump(std::stringstream &line);
    RESULT reset(std::stringstream &line);
    RESULT init(std::stringstream &line);
    RESULT load(std::stringstream &line);
    RESULT save(std::stringstream &line);
    RESULT edit(std::stringstream &line);
    RESULT reg(std::stringstream &line);
    RESULT reg_sub(std::string reg_name, UINT16 reg_value, int bytes);
    std::string reg_str();
    std::string regSR();
    RESULT unasm(std::stringstream &line);
    RESULT trace(std::stringstream &line);
    RESULT go(std::stringstream &line);
    RESULT bp(std::stringstream &line);
    RESULT bd(std::stringstream &line);
    RESULT bc(std::stringstream &line);
    RESULT be(std::stringstream &line);
    RESULT bl(std::stringstream &line);

    std::string bp_str(WORD addr);
    bool isBP(WORD addr);
    RESULT get_hex(std::stringstream &line, int &hex, int default_value = -1);
    RESULT get_dec(std::stringstream &line, int &dec, int default_value = -1);
    bool isEnd(std::stringstream &line);
};

#endif