#include <stdio.h>
#include <stdlib.h>
#include "NanoCpu.h"

/*
 *  ===== Table of ALU operations =====
 *  Must match order of ALU_OP...
 */
char szAlu[16][5] =
{
/*   0/8     1/9     2/A     3/B     4/C     5/D     6/E     7/F  */
    "add",  "sub",  "adc",  "sbc ", "rsub", "and",  "or ",  "xor",
    "mul",  "div",  "asr",  "lsr ", "aluc", "alud", "alue", "aluf"
};

/*
 *  ===== Table of Conditional Branch instructions =====
 *  Must match order of COND_BRA...
 */
char szBra[16][4] =
{
/*   0/8    1/9    2/A    3/B    4/C    5/D    6/E    7/F  */
    "beq", "bne", "bhi", "bls", "bhs", "blo", "bgt", "ble",
    "bge", "blt", "bra", "rts", "jal", "bdx", "bex", "bfx"
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
		length = SNPRINTF(line, len, "add  %s,%s,#%-3u", szRegName[Rx], szRegName[Ry], OPC_IMM4(opc));
        break;
    case OPC_SUB_IMM:
		length = SNPRINTF(line, len, "sub  %s,%s,#%-3u", szRegName[Rx], szRegName[Ry], OPC_IMM4(opc));
        break;
	case OPC_ADC_IMM:
		length = SNPRINTF(line, len, "adc  %s,%s,#%-3u", szRegName[Rx], szRegName[Ry], OPC_IMM4(opc));
		break;
	case OPC_SBC_IMM:
		length = SNPRINTF(line, len, "sbc  %s,%s,#%-3u", szRegName[Rx], szRegName[Ry], OPC_IMM4(opc));
		break;
	case OPC_RSUB_IMM:
		length = SNPRINTF(line, len, "rsub %s,%s,#%-3u", szRegName[Rx], szRegName[Ry], OPC_IMM4(opc));
		break;
	case OPC_AND_IMM:
		length = SNPRINTF(line, len, "and  %s,%s,#%-3u", szRegName[Rx], szRegName[Ry], OPC_IMM4(opc));
        break;
    case OPC_OR_IMM:
		length = SNPRINTF(line, len, "or   %s,%s,#%-3u", szRegName[Rx], szRegName[Ry], OPC_IMM4(opc));
        break;
    case OPC_XOR_IMM:
		length = SNPRINTF(line, len, "xor  %s,%s,#%-3u", szRegName[Rx], szRegName[Ry], OPC_IMM4(opc));
        break;
	case OPC_LB_OFF:
		length = SNPRINTF(line, len, "lb  %s,%u[%s]", szRegName[Rx], OPC_OFF4(opc), szRegName[Ry]);
		break;
	case OPC_SB_OFF:
		length = SNPRINTF(line, len, "sb  %s,%u[%s]", szRegName[Rx], OPC_OFF4(opc), szRegName[Ry]);
		break;
	case OPC_ALU_REG:
		length = SNPRINTF(line, len, "%s  %s,%s", szAlu[OPC_RZ(opc)], szRegName[Rx], szRegName[Ry]);
        break;
    case OPC_BRANCH:
		length = SNPRINTF(line, len, "%-4s $" NANO_SZADDR, szBra[Rx], addr + 2 * SIGN_EXT(opc, 128) + 2);
        break;
	case OPC_MOV_IMM:
		length = SNPRINTF(line, len, "mov %s,#%-3u", szRegName[Rx], OPC_IMM8(opc));
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
		length = SNPRINTF(line, len, ".word $%04X", opc);
        break;
    }
    return length;
}
