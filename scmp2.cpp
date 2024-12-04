#include <iostream>
#include <strings.h>
#include "common.h" 
#include "memory.hpp"
#include "cpu.hpp"
#include "monitor.hpp"
#include "disasm.hpp"

void go(CPU &cpu);

int main(int argc, char* argv[])
{
    Memory memory;
    CPU cpu(memory);
    Disasm disasm(memory, cpu);
    Monitor monitor(memory, cpu, disasm);

    if (argc == 1){
        monitor.monitor();      // enter monitor
    }
    else if (argc == 2){
        if (memory.load(argv[1]) == true){
            if (strcasecmp(argv[1], "nibl.srec") == 0){
                cpu.setSB();
            }
            go(cpu);            // exec
        }
    }
    else {
        std::cout << "Error" << std::endl;
    }

    return (0);
}

void go(CPU &cpu)
{
    CPUSTAT status;

    cpu.run_mode(RUN);

    do {
        status = cpu.clock();
    } while (status == SUCCESS);

    if (status == HALT){
        std::cout << "HALT!" << std::endl;
    }
    else if (status == UNDEFINED){
        std::cout << "UNDEFINED INSTRUCTION!" << std::endl;
    }
}
