#
#
#
.SUFFIXES:

.SUFFIXES:	.cpp .o

.cpp.o:
	g++ -c -Wall -O2 -o $*.o $*.cpp
#
#
files	= scmp2.o memory.o cpu.o inst1byte.o inst2byte.o monitor.o disasm.o util.o
#
#
#
#
scmp2.exe : $(files)
	g++ -O2 -s $(files) -o $@
clean:
	-rm *.o
	-rm *.exe
#
#
