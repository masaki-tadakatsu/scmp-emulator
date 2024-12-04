#include <iostream>
#include <iomanip>      // for std::setw, std::setfill>
#include <sstream>
#include <string>
#include <algorithm>
#include "common.h"
#include "util.hpp"
#include "memory.hpp"
#include "cpu.hpp"
#include "monitor.hpp"

using namespace std;


Monitor::Monitor(Memory &mem, CPU &cpu, Disasm &disasm): memory(mem), cpu(cpu), disasm(disasm)
{
    BPstat = BP_NONE;
}

void Monitor::monitor()
{
    stringstream line;
    string command;
    RESULT ret;

    while (1) {
        std::cout << ">>";
        std::getline(cin, command);
        transform(command.begin(), command.end(), command.begin(), ::toupper);

		line.str("");
		line.clear(std::stringstream::goodbit);
        line << command;
        std::getline(line, command, ' ');

        ret = OK;
        if (command == "Q"){
            break;
        }
        else if (command == "H" || command == "?"){
            ret = help(line);
        }
        else if (command == "D"){
            ret = dump(line);
        }
        else if (command == "RESET"){
            ret = reset(line);
        }
        else if (command == "INIT"){
            ret = init(line);
        }
        else if (command == "L"){
            ret = load(line);
        }
        else if (command == "S"){
            ret = save(line);
        }
        else if (command == "E"){
            ret = edit(line);
        }
        else if (command == "R"){
            ret = reg(line);
        }
        else if (command == "U"){
            ret = unasm(line);
        }
        else if (command == "T"){
            ret = trace(line);
        }
        else if (command == "G"){
            ret = go(line);
        }        
        else if (command == "BP"){
            ret = bp(line);
        }        
        else if (command == "BC"){
            ret = bc(line);
        }        
        else if (command == "BD"){
            ret = bd(line);
        }        
        else if (command == "BE"){
            ret = be(line);
        }        
        else if (command == "BL"){
            ret = bl(line);
        }
        else {
            cout << "Error!" << endl;
        }        
        if (ret == NG){
            cout << "Error!" << endl;
        }
    }
}

RESULT Monitor::help(stringstream &line)
{
    cout << "Reset CPUT : RESET" << endl;
    cout << "Init System: INIT" << endl;
    cout << "Trace      : T [steps]" << endl;
    cout << "Go         : G [addr]" << endl;
    cout << "Dump       : D [saddr] [eaddr]" << endl;
    cout << "Edit       : E [addr] [data]" << endl;
    cout << "Register   : R [reg-name]" << endl;
    cout << "Unassemble : U [addr] [steps]" << endl;
    cout << "Break Point: BP [addr]" << endl;
    cout << "Clear BP   : BC" << endl;
    cout << "Disable BP : BD" << endl;
    cout << "Enable BP  : BE" << endl;
    cout << "List BP    : BL" << endl;
    cout << "Load       : L [filename]" << endl;
    cout << "Save       : S [filename] [saddr] [eaddr]" << endl;
    cout << "Help       : H or ?" << endl;

    return (OK);
}

RESULT Monitor::dump(stringstream &line)
{
    int start, end;
    
    if (get_hex(line, start, cpu.getPC()) != OK){
        return (NG);
    }
    
    if (get_hex(line, end, start + 15) != OK){
        return (NG);
    }
    if (!isEnd(line)){
        return (NG);
    }

    if (end < start){
        return (NG);
    }
    memory.dump((WORD)start, (WORD)end);

    return (OK);
}

RESULT Monitor::get_hex(stringstream &line, int &hex, int default_value)
{
    string str;
    size_t* idx;

    if (!std::getline(line, str, ' ')){
        if (default_value == -1){
            return (NG);
        }
        hex = default_value;
        return (OK);
    }
    try {
        hex = std::stoi(str, idx, 16);
    }
    catch (const std::invalid_argument& e) {
        return (NG);
    }
    return (OK);
}

RESULT Monitor::get_dec(stringstream &line, int &dec, int default_value)
{
    string str;
    size_t* idx;

    if (!std::getline(line, str, ' ')){
        if (default_value == -1){
            return (NG);
        }
        dec = default_value;
        return (OK);
    }
    try {
        dec = std::stoi(str, idx, 10);
    }
    catch (const std::invalid_argument& e) {
        return (NG);
    }
    return (OK);
}

bool Monitor::isEnd(stringstream &line)
{
    string str;

    if (!std::getline(line, str)){
        return (true);
    }
    return (false);
}

RESULT Monitor::reset(stringstream &line)
{
    if (!isEnd(line)){
        return (NG);
    }

    cpu.reset();

    cout << Monitor::reg_str() << endl;

    return (OK);
}

RESULT Monitor::init(stringstream &line)
{
    if (!isEnd(line)){
        return (NG);
    }

    memory.clear();
    cpu.reset();

    cout << Monitor::reg_str() << endl;

    return (OK);
}

RESULT Monitor::load(stringstream &line)
{
    string filename;
    
    if (!std::getline(line, filename, ' ')){
        return (NG);
    }

    if (memory.load(filename) == false){
        return (NG);
    }
    if (!isEnd(line)){
        return (NG);
    }

    if (filename == "NIBL.SREC"){
        cpu.setSB();
    }

    return (OK);
}

RESULT Monitor::save(stringstream &line)
{
    string filename;
    int start, end;
    
    if (!std::getline(line, filename, ' ')){
        return (NG);
    }
   
    if (get_hex(line, start, -1) != OK){
        return (NG);
    }
    
    if (get_hex(line, end, -1) != OK){
        return (NG);
    }
    if (!isEnd(line)){
        return (NG);
    }

    if (end < start){
        return (NG);
    }

    if (memory.save(filename, start, end) == false){
        return (NG);
    }

    return (OK);
}

RESULT Monitor::edit(stringstream &line)
{
    int addr, data;
    string in;
    
    if (get_hex(line, addr, -1) != OK){
        return (NG);
    }
    
    if (get_hex(line, data, -2) != OK){
        return (NG);
    }
    if (!isEnd(line)){
        return (NG);
    }

    if (data != -2){
        cout << Util::hex2str((WORD)addr);
        cout << " ";
        cout << Util::hex2str(memory.read(addr));
        cout << ":";
        cout << Util::hex2str((BYTE)data);
        cout << endl;

        memory.write((WORD)addr, (BYTE)data);
        return (OK);
    }

    while (1){
        cout << Util::hex2str((WORD)addr);
        cout << " ";
        cout << Util::hex2str(memory.read(addr));
        cout << ":";
        cin >> in;

        if (in == "."){
            std::cin.ignore();
            break;
        }

        try {
            size_t idx;
            data = std::stoi(in, &idx, 16);
            if (in.length() != idx){
                continue;
            }
            memory.write((WORD)addr, (BYTE)data);
            addr++;
        }
        catch (const std::invalid_argument& e) {
            // nothing to do
        }
    }

    return (OK);
}

RESULT Monitor::reg(stringstream &line)
{
    string reg;
    RESULT result;

    if (!std::getline(line, reg, ' ')){
        cout << Monitor::reg_str() << endl;

        return (OK);
    }
    if (!isEnd(line)){
        return (NG);
    }

    if (reg == "AC"){
        result = reg_sub(reg, cpu.getAC(), 1);
    }
    else if (reg == "ER"){
        result = reg_sub(reg, cpu.getER(), 1);
    }
    else if (reg == "SR"){
        result = reg_sub(reg, cpu.getSR(), 1);
    }
    else if (reg == "PC"){
        result = reg_sub(reg, cpu.getPC(), 2);
    }
    else if (reg == "P1"){
        result = reg_sub(reg, cpu.getP1(), 2);
    }
    else if (reg == "P2"){
        result = reg_sub(reg, cpu.getP2(), 2);
    }
    else if (reg == "P3"){
        result = reg_sub(reg, cpu.getP3(), 2);
    }
    else {
        return (NG);
    }

    return (result);
}

std::string Monitor::reg_str()
{ 
    std::stringstream out; 

    out << regSR();
    out << " ";
    out << "PC:" << Util::hex2str(cpu.getPC());
    out << " ";
    out << "AC:" << Util::hex2str(cpu.getAC());
    out << " ";
    out << "ER:" << Util::hex2str(cpu.getER());
    out << " ";
    out << "P1:" << Util::hex2str(cpu.getP1());
    out << " ";
    out << "P2:" << Util::hex2str(cpu.getP2());
    out << " ";
    out << "P3:" << Util::hex2str(cpu.getP3());

    return (out.str());
}

std::string Monitor::regSR()
{
    string out = "COBAI210";

    for (int i = 0; i < 8; i++){
        if ((cpu.getSR() & (0x80 >> i)) == 0){
            out[i] = '-';
        }
    }
    return (out);
}

RESULT Monitor::reg_sub(string reg_name, UINT16 reg_value, int bytes)
{
    string in;
    UINT16 data;

    while (1){
        cout << reg_name;
        cout << " ";
        cout << setfill('0') << right << setw(bytes * 2) << hex << reg_value;
        cout << ":";
        cin >> in;

        if (in == "."){
            break;
        }

        try {
            size_t idx;
            data = std::stoi(in,  &idx, 16);
            if (in.length() != idx){
                continue;
            }
            if (reg_name == "AC"){
                cpu.setAC(data);
            }
            else if (reg_name == "ER"){
                cpu.setER(data);
            }
            else if (reg_name == "SR"){
                cpu.setSR(data);
            }
            else if (reg_name == "PC"){
                cpu.setPC(data);
            }
            else if (reg_name == "P1"){
                cpu.setP1(data);
            }
            else if (reg_name == "P2"){
                cpu.setP2(data);
            }
            else if (reg_name == "P3"){
                cpu.setP3(data);
            }
            break;
        }
        catch (const std::invalid_argument& e) {
            // nothing to do
        }
    }
    std::cin.ignore();

    return (OK);
}

RESULT Monitor::unasm(std::stringstream &line)
{
    int addr, steps;
    string assembler, ea;

    if (get_hex(line, addr, cpu.getPC() + 1) != OK){
        return (NG);
    }
    if (get_dec(line, steps, 8) != OK){
        return (NG);
    }
    if (!isEnd(line)){
        return (NG);
    }

    for (int i = 0; i < steps; i++){
        disasm.unasm(addr, assembler, ea);
        std::cout << bp_str(addr);
        cout << setfill(' ') << setw(13) << left << disasm.mem(addr);
        cout << assembler << endl;
        if ((memory.read(addr) & BIT_SIGN_BYTE) == 0){
            addr++;
        }
        else {
            addr += 2;
        }
    }

    return (OK);
}

RESULT Monitor::trace(std::stringstream &line)
{
    int steps, step;
    string assembler, ea_mem;

    if (get_dec(line, steps, 1) != OK){
        return (NG);
    }
    if (!isEnd(line)){
        return (NG);
    }
    cpu.run_mode(TRACE);
    for (step = 0; step < steps; step++){
        WORD addr = cpu.getPC() + 1;

        disasm.save_pr();
#if PREEXEC == 1
        disasm.unasm(addr, assembler, ea_mem);
        std::cout << bp_str(addr);
        std::cout << std::left << std::setfill(' ') << std::setw(13) << disasm.mem(addr);   
        std::cout << std::left << std::setfill(' ') << std::setw(16) << assembler;
        std::cout << std::left << std::setfill(' ') << std::setw(11) << ea_mem;

        cout << " : " << Monitor::reg_str() << std::endl;
#endif
        CPUSTAT status = cpu.clock();
#if PREEXEC == 0
        disasm.unasm(addr, assembler, ea_mem);
        std::cout << bp_str(addr);
        std::cout << std::left << std::setfill(' ') << std::setw(13) << disasm.mem(addr);   
        std::cout << std::left << std::setfill(' ') << std::setw(16) << assembler;
        std::cout << std::left << std::setfill(' ') << std::setw(11) << ea_mem;

        cout << " : " << Monitor::reg_str() << std::endl;
#endif
        if (status == INTERRPT){
            std::cout << "Interrpt!: ";
            std::cout << "PC(" << Util::hex2str(cpu.getP3()) << ")<->P3(" << Util::hex2str(cpu.getPC()) << ")" << std::endl;
        }
        if (status == HALT){
            std::cout << "HALT!" << std::endl;
            break;
        }
        else if (status == UNDEFINED){
            std::cout << "UNDEFINED INSTRUCTION!" << std::endl;
            break;
        }
        if (isBP(addr)){
            std::cout << "Break at " << Util::hex2str(addr) << std::endl;          
        }
    }

    return (OK);
}

RESULT Monitor::go(std::stringstream &line)
{
    CPUSTAT status;
    int addr;


    addr = cpu.getPC();

    if (get_hex(line, addr, -2) != OK){
        return (NG);
    }
    if (!isEnd(line)){
        return (NG);
    }
    if (addr == -1){
        cpu.setPC(addr);
    }

    cpu.run_mode(RUN);
    do {
        addr = cpu.getPC() + 1;

        status = cpu.clock();
        if (isBP((WORD)addr)){
            std::cout << "Break at " << Util::hex2str((WORD)addr) << std::endl;        
            break;
        }
    } while (status == SUCCESS);

    if (status == HALT){
        std::cout << "HALT!" << std::endl;
    }
    else if (status == UNDEFINED){
        std::cout << "UNDEFINED INSTRUCTION!" << std::endl;
    }

    return (OK);
}

RESULT Monitor::bp(std::stringstream &line)
{
    int addr;

    if (get_hex(line, addr, -1) != OK){
        return (NG);
    }
    if (!isEnd(line)){
        return (NG);
    }

    BPaddr = (WORD)addr;
    BPstat = BP_ENABLE;
    
    bl(line);

    return (OK);
}

RESULT Monitor::bd(std::stringstream &line)
{
    if (!isEnd(line)){
        return (NG);
    }
    if (BPstat != BP_NONE){
        BPstat = BP_DISABLE;
    }

    bl(line);

    return (OK);
}

RESULT Monitor::bc(std::stringstream &line)
{
    if (!isEnd(line)){
        return (NG);
    }

    BPstat = BP_NONE;

    bl(line);

    return (OK);
}

RESULT Monitor::be(std::stringstream &line)
{
    if (!isEnd(line)){
        return (NG);
    }

    if (BPstat != BP_NONE){
        BPstat = BP_ENABLE;
    }

    bl(line);

    return (OK);
}

RESULT Monitor::bl(std::stringstream &line)
{
    std::string out;

    if (!isEnd(line)){
        return (NG);
    }
    if (BPstat == BP_NONE){
        out = "No Break Point";
    }
    else {
        out = "BP=" + Util::hex2str(BPaddr) + ":";
        if (BPstat == BP_ENABLE){
            out += "Enable";
        }
        else if (BPstat == BP_DISABLE){
            out += "Disable";
        }
    }
    std::cout << out << endl;

    return (OK);
}

std::string Monitor::bp_str(WORD addr)
{
    std::string out;

    if (isBP(addr)) {
        if (BPstat == BP_ENABLE){
            out = "[*]";
        }
        else if (BPstat == BP_DISABLE){
            out = "[+]";
        }
    }
    else {
        out = "   ";
    }

    return (out);
}

bool Monitor::isBP(WORD addr)
{
    if (BPstat != BP_NONE &&
        ((BPaddr == addr) || (((memory.read(addr) & BIT_SIGN_BYTE) != 0) && (addr + 1 == BPaddr)))) {
        return (true);
    }
    return (false);
}