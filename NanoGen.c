#include <stdio.h>

#include "NanoCpu.h"

#define LENGTH      80
#define NANO_WORDS  32768

// Program to generate Nano test case
int main(int argc, const char* argv[])
{
    NANO_ADDR addr;
    for (addr = 0; addr < NANO_WORDS; addr += 7)
    {
        char line[LENGTH];
        NanoDisAsm(line, LENGTH, addr * 2, addr * 2);
        fprintf(stdout, "\t%s\n", line);
    }
    return 0;
}
