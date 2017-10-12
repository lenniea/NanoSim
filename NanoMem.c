#include "NanoCpu.h"

#define MEM_WORDS   32768

NANO_WORD led_out = 0xFF;
NANO_WORD sw_inp = 0xFF;

NANO_SHORT memory[MEM_WORDS];

#define ILLEGAL_ADDR(a)     (((a) & 1) || (a >= MEM_WORDS * 2))

int MemReadWord(NANO_ADDR addr, NANO_SHORT* data)
{
	if (ILLEGAL_ADDR(addr))
		return -1;
	*data = memory[addr >> 1];
	return 1;
}

int MemReadLong(NANO_ADDR addr, NANO_LONG* data)
{
	NANO_SHORT lo,hi;
	int result = MemReadWord(addr, &hi);
	if (result < 0)
		return result;
	result = MemReadWord((NANO_ADDR) (addr + 2), &lo);
	if (result < 0)
		return result;
	*data = ((NANO_LONG) hi << 16) | (lo);
	return 2;
}

int MemWriteWord(NANO_ADDR addr, NANO_SHORT data)
{
	if (ILLEGAL_ADDR(addr))
		return -1;
	memory[addr >> 1] = data;
	return 1;
}

int MemWriteLong(NANO_ADDR addr, NANO_LONG data)
{
	int result = MemWriteWord(addr, (NANO_SHORT) (data >> 16));
	if (result < 0)
		return result;
	return MemWriteWord((NANO_ADDR) (addr + 2), (NANO_SHORT) data);
}

void MemCopyBytes(NANO_ADDR addr, void* buf, int length)
{
	unsigned char* ptr = buf;
	while (length > 0)
	{
		NANO_SHORT word = (ptr[0] << 8) | ptr[1];
		MemWriteWord(addr, word);
		addr += sizeof(NANO_SHORT);
		ptr += sizeof(NANO_SHORT);
		length -= sizeof(NANO_SHORT);
	}
}

