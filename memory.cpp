#include <iostream>     // for std::cout
#include <iomanip>      // for std::setw, std::setfill>
#include <fstream>
#include <string>
#include "common.h"
#include "util.hpp"
#include "memory.hpp"

Memory::Memory()
{
    Memory::clear();
}

void Memory::clear()
{
    memory.fill(0);
}

void Memory::clear(BYTE data)
{
    memory.fill(data);
}

BYTE Memory::read(WORD addr)
{
    return (memory.at(addr));
}

void Memory::write(WORD addr, BYTE data)
{
    memory.at(addr) = data;
}

void Memory::dump(WORD start_addr, WORD end_addr)
{
    std::string char_str;
    BYTE data;

    std::cout << "addr +0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +a +b +c +d +e +f" << std::endl;
    for (int addr = start_addr & 0xfff0; addr <= end_addr; addr++){
        if ((addr & 0x0f) == 0){
            std::cout << Util::hex2str((WORD)addr);
        }
        if (addr < start_addr){
            std::cout << "   ";
            char_str += " ";
        }
        else {
            data = Memory::read(addr);
            std::cout << " " << Util::hex2str(data);
            if (' ' <= data && data <= '}'){
                char_str += data;
            }
            else {
                char_str += ".";
            }
        }
        if ((addr & 0x0f) == 0xf || addr == end_addr){
            std::cout << "  " << char_str << std::endl;
            char_str = "";
        }
    }
}

bool Memory::load(std::string filename)
{
    std::ifstream file;
    std::string line;

    file.open(filename);
    if (file.fail()){
        std::cout << "File not found!(" << filename << ")" << std::endl;
        return (false);
    }

    int start_addr = 0x0ffff;
    int end_addr = 0;
    while (getline(file, line)) {  // 1行ずつ読み込む
        if (file.fail()){
            std::cout << "Read ERROR!!" << filename << std::endl;
            file.close();
            return (false);
        }
        if (!Memory::check_csum(line)){
            std::cout << "Check sum ERROR!!" << std::endl;
            file.close();
            return (false);           
        }

        std::string record = line.substr(0, 2);
        int len = stoul(line.substr(2, 2), nullptr, 16);
        int addr = stoul(line.substr(4, 4), nullptr, 16);
        if (record == "S0"){
            // nothing to do
        }
        else if (record == "S1"){
            for (int i = 0; i < len - 3; i++){
                int data = stoul(line.substr(i * 2 + 8, 2), nullptr, 16);
                Memory::write(i + addr, (BYTE)data);
                if (start_addr > i + addr){
                    start_addr = i + addr;
                }
                if (end_addr < i + addr){
                    end_addr = i + addr;
                }
            }
        }
        else if (record == "S5"){
        
        }
        else if (record == "S9"){
            if (line != "S9030000FC"){
                std::cout << "FORMAT ERROR(S9)!!" << std::endl;
                file.close();
                return (false);
            }
        }
        else {
            std::cout << "FORMAT ERROR(unknown record)!!" << std::endl;
            file.close();
            return (false);
        }
    }

    file.close();

    std::cout << filename << "(";
    std::cout << Util::hex2str((WORD)start_addr);
    std::cout << ":";
    std::cout << Util::hex2str((WORD)end_addr);
    std::cout << ")" << std::endl;

    return (true);
}

bool Memory::check_csum(const std::string &line)
{
    int csum;

    csum = 0;
    for (int i = 0; i < stoi(line.substr(2, 2), nullptr, 16) + 1; i++){
        csum += stoul(line.substr(i * 2 + 2, 2), nullptr, 16);
        csum %= 256;
    }
    if (csum % 256 != 255){
        return (false);
    }
    return (true);
}

bool Memory::save(std::string filename, WORD start_addr, WORD end_addr)
{
    std::ofstream file;
    int csum;
    int count;

    file.open(filename);
    if (file.fail()){
        std::cout << "OPEN ERROR!!" << filename << std::endl;
        return (false);
    }

    // S0フィールド
    file << "S0";
    file << Util::hex2str_upper((BYTE)(filename.length() + 3));
    file << "0000";

    csum = filename.length() + 3;
    for (unsigned int i = 0; i < filename.length(); i++){
        file << Util::hex2str_upper((BYTE)filename.at(i));
        csum += (int)filename.at(i);
    }
    file << Util::hex2str_upper((BYTE)((csum ^ 0x0ff) & 0x0ff));
    file << std::endl;

    // S1フィールド
    count = 0;
    for (int addr = start_addr; addr <= end_addr; addr++){
        if ((addr == start_addr) || (addr % 16) == 0){
            count++;
            csum = 0;
            file << "S1";
            int bytes = 16;
            if (addr == start_addr){
                bytes -= start_addr;
            }
            if ((addr & 0xfff0) + 15 > end_addr){
                bytes -= (15 - (end_addr % 16));
            }
            bytes += 3;
            file << Util::hex2str_upper((BYTE)bytes);
            file << Util::hex2str_upper((WORD)addr);

            csum = bytes + (addr / 256) + (addr % 256);
        }

        int data = (int)Memory::read(addr);
        file << Util::hex2str_upper((BYTE)data);
        csum += data;

        if ((addr == end_addr) || (addr % 16) == 15){
            file << Util::hex2str_upper((BYTE)((csum ^ 0x0ff) & 0x0ff));
            file << std::endl;
        }
    }

    // S5フィールド
    file << "S503";
    file << Util::hex2str_upper((WORD)count);
    csum = 3 + (count / 256) + (count % 256);
    file << Util::hex2str_upper((BYTE)((csum ^ 0x0ff) & 0x0ff));
    file << std::endl;

    // S9フィールド
    file << "S9030000FC" << std::endl;

    if (file.fail()){
        std::cout << "Write ERROR!!" << std::endl;
        return (false);
    }

    file.close();

    std::cout << filename << "(";
    std::cout << Util::hex2str(start_addr);
    std::cout << ":";
    std::cout << Util::hex2str(end_addr);
    std::cout << ")" << std::endl;

    return (true);
}

