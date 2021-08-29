/*
 * (C) Copyright 2006-2011 Lennie Araki
 * All Rights Reserved.
 */

#include <stdio.h>		// for SNPRINTF
#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#include "NanoCpu.h"

#define BIT8            0x0100

#define SIGN(w)         ((w) & NANO_MSB)
#define SIGN_EXT(x,b)   (((x) & (b)) ? (((x) & ((b) - 1)) - (b)) : ((x) & ((b) - 1)))

#define NO_PREFIX       0

#define NANO_PREFIX(p)  (p->prefix != NO_PREFIX)

/*
 *  ALU Rx,#imm
 *   _______________________________________________________________
 *  |               |               |                               |
 *  |      alu      |       Rx      |             imm8              |
 *  |___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|
 *    F   E   D   C   B   A   9   8   7   6   5   4   3   2   1   0
 *
 *  ALU Rx,Rx,Ry
 *   _______________________________________________________________
 *  |               |               |               |               | 
 *  |       A       |       Rx      |      Ry       |      alu      |
 *  |___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|
 *    F   E   D   C   B   A   9   8   7   6   5   4   3   2   1   0
 *
 *  JAL Rx,addr
 *   _______________________________________________________________
 *  |               |               |                               |
 *  |     opcode    |       Rx      |             imm8              |
 *  |___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|
 *    F   E   D   C   B   A   9   8   7   6   5   4   3   2   1   0
 *
 *   Cond Branch
 *   _______________________________________________________________
 *  |               |               |                               |
 *  |     opcode    |      cond     |             disp8             |
 *  |___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|
 *    F   E   D   C   B   A   9   8   7   6   5   4   3   2   1   0
 *
 *   Imm Prefix
 *   _______________________________________________________________
 *  |               |                                               |
 *  |     opcode    |                    imm12                      |
 *  |___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|
 *    F   E   D   C   B   A   9   8   7   6   5   4   3   2   1   0
 */

#define GET_OPC(opc)	(((opc) >> NANO_FUNC) & 0x0f)
#define OPC_COND(opc)   (((opc) >> NANO_RX) & 0x0f)

#define OPC_RX(opc)     (((opc) >> NANO_RX) & 0x0f)
#define OPC_RY(opc)     (((opc) >> NANO_RY) & 0x0f)
#define OPC_RZ(opc)     ((opc) & 0x0f)

#define OPC_OFF4(opc)   (((opc) & 0x0f)*2)
#define OPC_IMM8(opc)   ((opc) & 0xff)

#define OPC_IMM12(opc)  ((opc) & 0xFFF)

#define SHIFT_MASK      0x001f

/* eval 0-extended word offset prefix immediate */
#define PREFIX_IMM_WORD(p, opc) \
    (((p)->prefix << 5) | OPC_OFF4(opc))

/* eval word effective address imm(ra) */
#define WORD_EA(p, opc) \
    ((NANO_ADDR)((p)->reg[OPC_RB(opc)] + PREFIX_IMM_WORD(p, opc)))

#define WRITE_REG(p,dst,x)  { (p)->reg[dst] = (x); }

/* Fetch byte at addr. */
NANO_WORD NanoLoadByte(NANO_CPU* p, NANO_ADDR addr)
{
    NANO_SHORT data;
    int cycles = MemReadWord(addr & ~1, &data);
	if ((addr & 1) == 0)
		data = data << 8;
	// Sign extend into lower 8 bits
	data = (signed short)data >> 8;
    p->cycles += cycles;
    return data;
}

/* Fetch word at addr. */
NANO_WORD NanoLoadWord(NANO_CPU* p, NANO_ADDR addr)
{
    NANO_SHORT data;
    int cycles = MemReadWord(addr, &data);
    p->cycles += cycles;
    return data;
}

/* Fetch long at addr. */
NANO_LONG NanoLoadLong(NANO_CPU* p, NANO_ADDR addr)
{
    NANO_LONG data;
    int cycles = MemReadLong(addr, &data);
    p->cycles += cycles;
    return data;
}

/* Store byte at addr. */
void NanoStoreByte(NANO_CPU* p, NANO_ADDR addr, NANO_SHORT data)
{
    int cycles = MemWriteByte(addr, data);
    p->cycles += cycles;
}

/* Store word at addr. */
void NanoStoreWord(NANO_CPU* p, NANO_ADDR addr, NANO_SHORT data)
{
    int cycles = MemWriteWord(addr, data);
    p->cycles += cycles;
}

/* Store long at addr. */
void NanoStoreLong(NANO_CPU* p, NANO_ADDR addr, NANO_LONG data)
{
    int cycles = MemWriteLong(addr, data);
    p->cycles += cycles;
}

/* Report illegal opcode. */
void NanoIllegalOpcode(NANO_CPU* p)
{
}

/* Return processor state following reset.
 *
 * Args
 *  trace   - initial trace flags
 */
void NanoReset(NANO_CPU* p)
{
    memset(p, 0, sizeof(NANO_CPU));
    p->prefix = NO_PREFIX;
#if defined(_DEBUG)
	p->ccr = NANO_N | NANO_C | NANO_V | NANO_Z;
#endif
}

/* Macro to compute the sum of a+b+carry and return carry out */
#define ALU_ADDSUB(r, a, b, carry) \
    r = (a) + (b) + (carry); \
    carry = carry ? (r <= (a)) || (r <= (b)) : (r < (a)) || (r < (b));

void NanoAluOp(NANO_CPU* p, NANO_ALU alu, int Rx, NANO_WORD a, NANO_WORD b)
{
    NANO_WORD result;
    NANO_WORD cond;
    NANO_WORD carry;

    carry = (p->ccr & NANO_C) ? 1 : 0;

    switch (alu)
	{
    case ALU_ADD:
		carry = 0;
    case ALU_ADC:   /* Add w/ Carry */
        ALU_ADDSUB(result, a, b, carry);
        WRITE_REG(p, Rx, result);
        break;
	case ALU_SUB:
		carry = 0;
    case ALU_SBC:   /* Subtract w/ Carry */
        carry = carry ? 0 : 1;
        b = ~b;
        ALU_ADDSUB(result, a, b, carry);
        carry = !carry;
        WRITE_REG(p, Rx, result);
        break;
	case ALU_RSUB:
		result = b - a;
		WRITE_REG(p, Rx, result);
		break;
    case ALU_AND:   /* And */
        result = a & b;
        WRITE_REG(p, Rx, result);
        break;
    case ALU_OR:    /* Or */
        result = a | b;
        WRITE_REG(p, Rx, result);
        break;
    case ALU_XOR:   /* eXclusive Or */
        result = a ^ b;
        WRITE_REG(p, Rx, result);
        break;
#if 0
    case ALU_SHIFT: /* SHIFT */
        b = b & SHIFT_MASK;
        for (result = a; b != 0; --b)
        {
            carry = (result & 1) ? 1 : 0;
            if (b & NANO_SHIFT_LEFT)
                result = ((NANO_SWORD) result << 1);
            else
                result = ((NANO_WORD) result >> 1);
        }
        WRITE_REG(p, Rx, result);
        break;
    case ALU_LSH:   /* Logical Shift */
        b = b & SHIFT_MASK;
        for (result = a; b != 0; --b)
        {
            carry = (result & 1) ? 1 : 0;
            if (b & NANO_SHIFT_LEFT)
                result = ((NANO_WORD) result << 1);
            else
                result = ((NANO_WORD) result >> 1);
        }
        WRITE_REG(p, Rx, result);
        break;
#endif
    }
    /* Update Condition Codes based on result, a & b */
    cond = (result & NANO_MSB) ? NANO_N : 0;

    /* overflow if the sign of the result is different from the signs of both operands */
    if (SIGN(a ^ result) && SIGN(b ^ result))
        cond |= NANO_V;

    if (result == 0)
        cond |= NANO_Z;

    if (carry)
        cond |= NANO_C;
	// Update condition codes
	p->ccr = cond;
}

/* Local function to find length of instruction */
static int InstLength(NANO_ADDR addr)
{
    int length = 0;
    int type;
    /* count instructions until non-prefix */
    do
    {
        NANO_INST opc;
        MemReadWord(addr, &opc);    /* Fetch opcode */
        addr += 2;
        type = GET_OPC(opc);
        length += 2;
    }
    while (type == OPC_IMM);
    return length;
}

int NanoTestCond(NANO_CPU* p, int cond)
{
    int br;
    int data;

    switch (cond)
    {
    case COND_BRA:  /* true */
        br = 1;
        break;
    case COND_BHI:  /* c | z=0 */
        br = (p->ccr & (NANO_C|NANO_Z)) == 0;
        break;
    case COND_BLS:  /* c | z=1 */
        br = (p->ccr & (NANO_C|NANO_Z));
        break;
    case COND_BHS:  /* c=0 */
        br = (p->ccr & NANO_C) == 0;
        break;
    case COND_BLO:  /* c=1 */
        br = (p->ccr & NANO_C);
        break;
    case COND_BEQ:  /* z=1 */
        br = (p->ccr & NANO_Z);
        break;
    case COND_BNE:  /* z=0 */
        br = (p->ccr & NANO_Z) == 0;
        break;
    case COND_BGE:  /* n^v=0 */
        data = p->ccr & (NANO_N|NANO_V);
        br = (data == 0) || (data == (NANO_N|NANO_V));
        break;
    case COND_BLT:  /* n^v=1 */
        data = p->ccr & (NANO_N|NANO_V);
        br = (data == NANO_N) || (data == NANO_V);
        break;
    case COND_BGT:  /* z|(n^v)=0 */
        data = p->ccr & (NANO_N|NANO_V|NANO_Z);
        br = (data == 0) || (data == (NANO_N|NANO_V));
        break;
    case COND_BLE:  /* z|(n^v)=1 */
        data = p->ccr & (NANO_N|NANO_V|NANO_Z);
        br = (data != 0) && (data != (NANO_N|NANO_V));
        break;
	case COND_RET:
		// TODO: implement RETurn
		br = 1;
		break;
    default:
        br = 0;
    }
    return br;
}


/*
 *  ===== NanoSimInst =====
 *      Simulate one or more CPU instructions. Note: prefixes are treated as
 *  separate instructions to mimic the behaviour of the hardware.
 */
typedef enum
{
    NANO_STOP, NANO_RUN
} NANO_CPU_STATE;

NANO_CPU_STATE cpuState = NANO_RUN;

int NanoSimInst(NANO_CPU* p, NANO_STEP step)
{
    NANO_ADDR breakpt;
    int count = 1000000;
    switch (step)
    {
    case NANO_STEP_OVER:
        breakpt = p->pc + InstLength(p->pc);
        break;
    case NANO_STEP_OUT:
        breakpt = p->reg[15];
        break;
    default:
        breakpt = 0;
    }
    cpuState = NANO_RUN;
    while (cpuState == NANO_RUN) {
        /* Fetch 16-bit instruction opcode */
        NANO_INST opc;
        NANO_ADDR addr;
        NANO_WORD data;
        int Rx,Ry,Rz;

        int cycles = MemReadWord(p->pc, &opc);
        p->cycles += cycles;

        p->pc += 2;

		/* Decode (register) fields */
		Rx = OPC_RX(opc);
		Ry = OPC_RY(opc);
		Rz = OPC_RZ(opc);

        switch (GET_OPC(opc))
		{
        case OPC_ADD_IMM:
			p->ccr &= ~NANO_C; // Clear CARRY
            data = ((p)->prefix << 8) | OPC_IMM8(opc);

            NanoAluOp(p, ALU_ADC, Rx, p->reg[Rx], data);
            p->prefix = NO_PREFIX;
            break;
        case OPC_SUB_IMM:
			p->ccr &= ~NANO_C; // Clear CARRY
            data = ((p)->prefix << 8) | OPC_IMM8(opc);

            NanoAluOp(p, ALU_ADC, Rx, p->reg[Rx], data);
            p->prefix = NO_PREFIX;
            break;
		case OPC_ADC_IMM:
			data = ((p)->prefix << 8) | OPC_IMM8(opc);

			NanoAluOp(p, ALU_ADC, Rx, p->reg[Rx], data);
			p->prefix = NO_PREFIX;
			break;
		case OPC_SBC_IMM:
			data = ((p)->prefix << 8) | OPC_IMM8(opc);

			NanoAluOp(p, ALU_SBC, Rx, p->reg[Rx], data);
			p->prefix = NO_PREFIX;
			break;
		case OPC_RSUB_IMM:
			p->ccr &= ~NANO_C; // Clear CARRY
			data = ((p)->prefix << 8) | OPC_IMM8(opc);

			NanoAluOp(p, ALU_RSUB, Rx, p->reg[Rx], data);
			p->prefix = NO_PREFIX;
			break;
		case OPC_AND_IMM:
			p->ccr &= ~NANO_C; // Clear CARRY
            data = ((p)->prefix << 8) | OPC_IMM8(opc);

            NanoAluOp(p, ALU_AND, Rx, p->reg[Rx], data);
            p->prefix = NO_PREFIX;
            break;
        case OPC_OR_IMM:
			p->ccr &= ~NANO_C; // Clear CARRY
            data = ((p)->prefix << 8) | OPC_IMM8(opc);

            NanoAluOp(p, ALU_OR, Rx, p->reg[Rx], data);
            p->prefix = NO_PREFIX;
            break;
        case OPC_XOR_IMM:
			p->ccr &= ~NANO_C; // Clear CARRY
            data = ((p)->prefix << 8) | OPC_IMM8(opc);

            NanoAluOp(p, ALU_XOR, Rx, p->reg[Rx], data);
            p->prefix = NO_PREFIX;
            break;
        case OPC_ALU_REG:
            NanoAluOp(p, Rz, Rx, p->reg[Rx], Ry);
            p->prefix = NO_PREFIX;
            break;
		case OPC_LB_OFF:
			addr = p->reg[Ry] + OPC_OFF4(opc)*2;
			data = NanoLoadByte(p, addr);
			WRITE_REG(p, Rx, data);
			break;
		case OPC_SB_OFF:
			addr = p->reg[Ry] + OPC_OFF4(opc);
			data = p->reg[Rx];
			NanoStoreByte(p, addr, data);
			break;
		case OPC_MOV_IMM:
			p->ccr &= ~NANO_C; // Clear CARRY
			data = ((p)->prefix << 8) | OPC_IMM8(opc);
			WRITE_REG(p, Rx, data);
			p->prefix = NO_PREFIX;
			break;
		case OPC_LW_OFF:
			addr = p->reg[Ry] + OPC_OFF4(opc);
			if (addr & 1)
			{
				// Load Byte
				data = NanoLoadByte(p, addr);
			}
			else
			{
				data = NanoLoadWord(p, addr);
			}
			WRITE_REG(p, Rx, data);
			break;
		case OPC_SW_OFF:
			addr = p->reg[Ry] + OPC_OFF4(opc)*2;
			data = p->reg[Rx];
			if (addr & 1)
			{
				NanoStoreByte(p, addr, data);
			}
			else
			{
				NanoStoreWord(p, addr, data);
			}
			break;
        case OPC_BRANCH:
		{
            int br = NanoTestCond(p, OPC_COND(opc));
            if (br) {
                p->pc += 2*SIGN_EXT(OPC_IMM8(opc), 0x80);
                p->cycles += 2;
            }
            break;
        }
        case OPC_IMM:
            p->prefix = (p->prefix << 12) | OPC_IMM12(opc);
            break;
        default:
            NanoIllegalOpcode(p);
            break;
        }

        /* Stop on breakpoint(s) or single step */
        if (p->pc == breakpt || p->pc == p->breakpoint ||
            step == NANO_STEP_INTO || --count == 0)
            break;
    }
    return 0;
}

/*
 *  ===== Table of ALU operations =====
 *  Must match order of ALU_OP...
 */
char szAlu[16][5] =
{
/*   0/8     1/9     2/A     3/B     4/C     5/D     6/E     7/F  */
    "add",  "sub",  "adc",  "sbc ", "rsub",  "and",  "or ",  "xor",
    "add",  "sub",  "adc",  "sbc ", "rsub",  "and",  "or ",  "xor"
};

/*
 *  ===== Table of Conditional Branch instructions =====
 *  Must match order of COND_BRA...
 */
char szBra[16][4] =
{
/*   0/8    1/9    2/A    3/B    4/C    5/D    6/E    7/F  */
    "beq", "bne", "bhi", "bls", "bhs", "blo", "bgt", "ble",
    "bge", "blt", "bra", "jmp", "?BC", "?BD", "?BE", "?BF"
};

char szRegName[16][4] =
{
/*   0/8    1/9    2/A    3/B    4/C    5/D    6/E    7/F  */
    "r0",  "r1",  "r2",  "r3",  "r4",  "r5",  "r6",  "r7",
    "r8",  "r9",  "r10", "r11", "r12", "r13", "sp",  "r15"
};

#ifdef _MSC_VER
#define SNPRINTF	_snprintf
#else
#define SNPRINTF	snprintf
#endif

/*
 *  ===== NanoDisAsm =====
 *      Disassemble one instruction.  Note: prefixes are treated as
 *  separate instructions to mimic the behaviour of the hardware.
 */
int NanoDisAsm(char* line, size_t len, NANO_ADDR addr, NANO_INST opc)
{
    int length;
	int Rx, Ry;

	/* Decode (register) fields */
	Rx = OPC_RX(opc);
	Ry = OPC_RY(opc);

    switch (GET_OPC(opc))
	{
    case OPC_ADD_IMM:
		length = SNPRINTF(line, len, "add  %s,#%u", szRegName[Rx], OPC_IMM8(opc));
        break;
    case OPC_SUB_IMM:
		length = SNPRINTF(line, len, "sub  %s,#%u", szRegName[Rx], OPC_IMM8(opc));
        break;
	case OPC_ADC_IMM:
		length = SNPRINTF(line, len, "adc  %s,#%u", szRegName[Rx], OPC_IMM8(opc));
		break;
	case OPC_SBC_IMM:
		length = SNPRINTF(line, len, "sbc  %s,#%u", szRegName[Rx], OPC_IMM8(opc));
		break;
	case OPC_RSUB_IMM:
		length = SNPRINTF(line, len, "rsub %s,#%u", szRegName[Rx], OPC_IMM8(opc));
		break;
	case OPC_AND_IMM:
		length = SNPRINTF(line, len, "and  %s,#%u", szRegName[Rx], OPC_IMM8(opc));
        break;
    case OPC_OR_IMM:
		length = SNPRINTF(line, len, "or   %s,#%u", szRegName[Rx], OPC_IMM8(opc));
        break;
    case OPC_XOR_IMM:
		length = SNPRINTF(line, len, "xor  %s,#%u", szRegName[Rx], OPC_IMM8(opc));
        break;
	case OPC_LB_OFF:
		length = SNPRINTF(line, len, "lb  %s,%u[%s]", szRegName[Rx], OPC_OFF4(opc), szRegName[Ry]);
		break;
	case OPC_SB_OFF:
		length = SNPRINTF(line, len, "sb  %s,%u[%s]", szRegName[Rx], OPC_OFF4(opc), szRegName[Ry]);
		break;
	case OPC_ALU_REG:
		length = SNPRINTF(line, len, "%s  %s,%s", szAlu[Rx], szRegName[Rx], szRegName[Ry]);
        break;
    case OPC_BRANCH:
		length = SNPRINTF(line, len, "%-4s " NANO_SZADDR, szBra[Rx], addr + 2 * SIGN_EXT(opc, 128) + 2);
        break;
	case OPC_MOV_IMM:
		length = SNPRINTF(line, len, "mov %s,#%u", szRegName[Rx], OPC_IMM8(opc));
		break;
	case OPC_LW_OFF:
		length = SNPRINTF(line, len, "lw  %s,%u[%s]", szRegName[Rx], OPC_OFF4(opc)*2, szRegName[Ry]);
        break;
    case OPC_SW_OFF:
		length = SNPRINTF(line, len, "sw  %s,%u[%s]", szRegName[Rx], OPC_OFF4(opc)*2, szRegName[Ry]);
        break;
    case OPC_IMM:
		length = SNPRINTF(line, len, "imm #%03x", OPC_IMM12(opc));
        break;
    default:
		length = SNPRINTF(line, len, "%04x???", opc);
        break;
    }
    return length;
}
