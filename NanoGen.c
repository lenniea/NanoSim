#include <stdio.h>

#include "NanoCpu.h"

#define LENGTH      80
#define NANO_WORDS  0xF000

// Program to generate Nano test case
int main(int argc, const char* argv[])
{
    int skip = (argc == 2) ? atoi(argv[1]) : 17;
        
    NANO_ADDR opc;
    for (opc = 0; opc < NANO_WORDS; opc += skip)
    {
        char line[LENGTH];
        NANO_ADDR addr = opc * 2;
        NanoDisAsm(line, LENGTH, addr * 2, opc);
        fprintf(stdout, "\t%s\t\t; %04X\n", line, opc);
    }
    return 0;
}
