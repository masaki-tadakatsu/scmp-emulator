#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <string>
#include <array>
#include "common.h"

class Memory {

public:
    Memory();
    void clear();
    void clear(BYTE data);
    BYTE read(WORD addr);
    void write(WORD addr, BYTE data);
    void dump(WORD start_addr = 0, WORD end_addr = 0xffff);
    bool load(std::string filename);
    bool save(std::string filename, WORD start_addr = 0, WORD end_addr = 0xffff);

private:
    bool check_csum(const std::string &line);

	std::array<BYTE, 64 * 1024> memory;
};

#endif