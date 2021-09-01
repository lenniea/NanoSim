#include <stdio.h>

#include "NanoCpu.h"

#define LENGTH      80
#define NANO_WORDS  0xF000

// Program to generate Nano test case
int main(int argc, const char* argv[])
{
    int skip = (argc == 2) ? atoi(argv[1]) : 17;
        
    NANO_ADDR w;
    for (w = 0; w < NANO_WORDS / skip; ++w)
    {
        char line[LENGTH];
        NANO_ADDR addr = w * 2;
        NANO_WORD opc = w * skip;
        NanoDisAsm(line, LENGTH, addr, opc);
        fprintf(stdout, "\t%s\t\t; %04X\n", line, opc);
    }
    return 0;
}
