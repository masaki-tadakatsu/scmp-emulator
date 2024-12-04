#ifndef CPU_HPP
#define CPU_HPP

#include "common.h"

// CPU run mode
enum CPUMODE {
    RUN,
    TRACE
};

// mask for opcode
const WORD BIT_OPCODE_PR   = 0x03;
const WORD BIT_OPCODE_MODE = 0x04;

// mask for pointer
const WORD BIT_PR_PAGE = 0xf000;

// opcode
// single-byte instruction
const	BYTE	OPE_HALT = 0x00;
const	BYTE	OPE_XAE	 = 0x01;
const	BYTE	OPE_CCL  = 0x02;
const	BYTE	OPE_SCL	 = 0x03;
const	BYTE	OPE_DINT = 0x04;
const	BYTE	OPE_IEN	 = 0x05;
const	BYTE	OPE_CSA	 = 0x06;
const	BYTE	OPE_CAS	 = 0x07;
const	BYTE	OPE_NOP	 = 0x08;
const	BYTE	OPE_UND	 = 0x09;
const	BYTE	OPE_SIO	 = 0x19;
const	BYTE	OPE_SR	 = 0x1C;
const	BYTE	OPE_SRL	 = 0x1D;
const	BYTE	OPE_RR	 = 0x1E;
const	BYTE	OPE_RRL	 = 0x1F;
const	BYTE	OPE_XPAL = 0x30;
const	BYTE	OPE_XPAH = 0x34;
const	BYTE	OPE_XPPC = 0x3C;
const	BYTE	OPE_LDE	 = 0x40;
const	BYTE	OPE_ANE	 = 0x50;
const	BYTE	OPE_ORE	 = 0x58;
const	BYTE	OPE_XRE	 = 0x60;
const	BYTE	OPE_DAE	 = 0x68;
const	BYTE	OPE_ADE	 = 0x70;
const	BYTE	OPE_CAE	 = 0x78;
const	BYTE	OPE_PUTC = 0x20;    /* for NIBL */
const	BYTE	OPE_GETC = 0x21;    /* for NIBL */

// double-byte instruction
const	BYTE	OPE_DLY	= 0x8F;
const	BYTE	OPE_JMP	= 0x90;
const	BYTE	OPE_JP	= 0x94;
const	BYTE	OPE_JZ	= 0x98;
const	BYTE	OPE_JNZ	= 0x9C;
const	BYTE	OPE_ILD	= 0xA8;
const	BYTE	OPE_DLD	= 0xB8;
const	BYTE	OPE_LD	= 0xC0;
const	BYTE	OPE_LDI	= 0xC4;
const	BYTE	OPE_ST	= 0xC8;
const	BYTE	OPE_AND	= 0xD0;
const	BYTE	OPE_ANI	= 0xD4;
const	BYTE	OPE_OR	= 0xD8;
const	BYTE	OPE_ORI	= 0xDC;
const	BYTE	OPE_XOR	= 0xE0;
const	BYTE	OPE_XRI	= 0xE4;
const	BYTE	OPE_DAD	= 0xE8;
const	BYTE	OPE_DAI	= 0xEC;
const	BYTE	OPE_ADD	= 0xF0;
const	BYTE	OPE_ADI	= 0xF4;
const	BYTE	OPE_CAD	= 0xF8;
const	BYTE	OPE_CAI	= 0xFC;

// bits of Status Resistor
const BYTE BIT_SR_CY = (1 << 7);
const BYTE BIT_SR_OV = (1 << 6);
const BYTE BIT_SR_SB = (1 << 5);
const BYTE BIT_SR_SA = (1 << 4);
const BYTE BIT_SR_IE = (1 << 3);
const BYTE BIT_SR_F2 = (1 << 2);
const BYTE BIT_SR_F1 = (1 << 1);
const BYTE BIT_SR_F0 = (1 << 0);

// CPU status after execute instruction
enum CPUSTAT {
    SUCCESS,
    HALT,
    INTERRPT,
    UNDEFINED
};

// CPU-class
class CPU {
public:
    CPU(Memory& mem);

    void reset();
    CPUSTAT clock();
    CPUSTAT interrupt();
    void run_mode(CPUMODE mode);

    // Sense-A,B pins
    inline void setSA(){reg.SR |= BIT_SR_SA;};
    inline void resetSA(){reg.SR &= ~BIT_SR_SA;};
    inline void setSB(){reg.SR |= BIT_SR_SB;};
    inline void resetSB(){reg.SR &= ~BIT_SR_SB;};

    // get register
    inline BYTE getAC(){return (reg.AC);};
    inline BYTE getER(){return (reg.ER);};
    inline WORD getPC(){return (reg.PR[0]);};
    inline WORD getP1(){return (reg.PR[1]);};
    inline WORD getP2(){return (reg.PR[2]);};
    inline WORD getP3(){return (reg.PR[3]);};
    inline BYTE getSR(){return (reg.SR);};

    // set register
    inline void setAC(BYTE data){reg.AC = data;};
    inline void setER(BYTE data){reg.ER = data;};
    inline void setPC(WORD data){reg.PR[0] = data;};
    inline void setP1(WORD data){reg.PR[1] = data;};
    inline void setP2(WORD data){reg.PR[2] = data;};
    inline void setP3(WORD data){reg.PR[3] = data;};
    inline void setSR(BYTE data){reg.SR = data;};

private:
    Memory &memory;     // memory clss instance

    struct {
        BYTE AC;
        BYTE ER;
        BYTE SR;
        WORD PR[4];
    } reg;

    CPUMODE runmode;

    BYTE fetch();
    WORD get_ea(int addressing, SBYTE disp);
    WORD calc_ea(int pr, SBYTE disp);
    SBYTE get_data(int addressing, SBYTE disp);
    BYTE add_byte(BYTE a, BYTE b);
    BYTE add_bcd(BYTE a, BYTE b);

    CPUSTAT exec(BYTE opcode);                // execute single-byte instruction
    CPUSTAT exec(BYTE opcode, SBYTE disp);    // execute double-byte instruction

    // Dingle-Byte Instruction
	CPUSTAT execHALT(BYTE opcode);
	CPUSTAT execXAE(BYTE opcode);
	CPUSTAT execCCL(BYTE opcode);
	CPUSTAT execSCL(BYTE opcode);
	CPUSTAT execDINT(BYTE opcode);
	CPUSTAT execIEN(BYTE opcode);
	CPUSTAT execCSA(BYTE opcode);
	CPUSTAT execCAS(BYTE opcode);
	CPUSTAT execNOP(BYTE opcode);
	CPUSTAT execSIO(BYTE opcode);
	CPUSTAT execSR(BYTE opcode);
	CPUSTAT execSRL(BYTE opcode);
	CPUSTAT execRR(BYTE opcode);
	CPUSTAT execRRL(BYTE opcode);
	CPUSTAT execXPAL(BYTE opcode);
	CPUSTAT execXPAH(BYTE opcode);
	CPUSTAT execXPPC(BYTE opcode);
	CPUSTAT execLDE(BYTE opcode);
	CPUSTAT execANE(BYTE opcode);
	CPUSTAT execORE(BYTE opcode);
	CPUSTAT execXRE(BYTE opcode);
	CPUSTAT execDAE(BYTE opcode);
	CPUSTAT execADE(BYTE opcode);
	CPUSTAT execCAE(BYTE opcode);
	CPUSTAT execPUTC(BYTE opcode);  // for NIBL
	CPUSTAT execGETC(BYTE opcode);  // for NIBL

    // Double-Byte Instruction
    CPUSTAT execDLY(BYTE opcode, SBYTE disp);
    CPUSTAT execJMP(BYTE opcode, SBYTE disp);
    CPUSTAT execJP(BYTE opcode, SBYTE disp);
    CPUSTAT execJZ(BYTE opcode, SBYTE disp);
    CPUSTAT execJNZ(BYTE opcode, SBYTE disp);
    CPUSTAT execILD(BYTE opcode, SBYTE disp);
    CPUSTAT execDLD(BYTE opcode, SBYTE disp);
    CPUSTAT execLD(BYTE opcode, SBYTE disp);    // LD, LDI
    CPUSTAT execST(BYTE opcode, SBYTE disp);
    CPUSTAT execAND(BYTE opcode, SBYTE disp);   // AND, ADI
    CPUSTAT execOR(BYTE opcode, SBYTE disp);    // OR, ORI
    CPUSTAT execXOR(BYTE opcode, SBYTE disp);   // XOR, XRI
    CPUSTAT execDAD(BYTE opcode, SBYTE disp);   // DAD, DAI
    CPUSTAT execADD(BYTE opcode, SBYTE disp);   // ADD, ADI
    CPUSTAT execCAD(BYTE opcode, SBYTE disp);   // CAD, CAI
};

#endif
