/* nanocpu.h */

#ifndef __NANOCPU_H__
#define __NANOCPU_H__

#include <stdlib.h>			// for size_t
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define NANO_FUNC		12
#define NANO_RX			8
#define NANO_RY			4
#define NANO_RZ			0

/*
 *  Arithmetic Logic Unit
 */

typedef enum
{
	ALU_ADD   =  0,		/* ADD */
	ALU_SUB   =  1,		/* SUBtract */
	ALU_ADC   =  2,		/* Add with Carry */
	ALU_SBC   =  3,		/* SuBtract with Carry */
	ALU_RSUB  =  4,		/* Reverse SUBtract */
	ALU_AND   =  5,		/* AND */
	ALU_OR    =  6,		/* inclusive OR */
	ALU_XOR   =  7,		/* eXclusive OR */

	ALU_MUL   =  8,		/* MULtiply */
	ALU_DIV   =  8,		/* reserved for DIV */
	ALU_ASR   =  10,	/* Arithmetic Shift Right */
	ALU_LSR   =  11,	/* Logical Shift Right */
	ALU_CX    =  12,	/* reserved (CX) */
	ALU_DX    =  13,	/* reserved (DX) */
	LINK      =  14,	/* LINK Rd = PC */
	LD_IMM    =  15,	/* LD IMM (upper 16-bits) */

} NANO_ALU;

/*
 *  Conditional Branch
 */
typedef enum
{
	COND_BEQ,		/* Branch EQual */
	COND_BNE,		/* Branch Not Equal */
	COND_BHI,		/* Branch HIgher */
	COND_BLS,		/* Branch Lower/Same */
	COND_BHS,		/* Branch Higher/Same */
	COND_BLO,		/* Branch LOwer */
	COND_BGT,		/* Branch if Greater Than */
	COND_BLE,		/* Branch if Less/Equal */
	COND_BGE,		/* Branch if Greater/Equal */
	COND_BLT,		/* Branch if Less Than */
	COND_BRA,		/* BRanch Always */
	COND_RTS,		/* ReTurn Subroutine */
	COND_JAL,		/* Jump And Link */
	COND_BD,		/* Branch reserved D */
	COND_BE,		/* Branch reserved E */
	COND_BF,		/* Repeat reserved F */
} BRANCH_COND;

/*
 *  4-bit "opcode" field
 */
typedef enum
{
	OPC_ADD_IMM  = 0,
	OPC_SUB_IMM  = 1,
	OPC_ADC_IMM  = 2,
	OPC_SBC_IMM  = 3,
	OPC_RSUB_IMM = 4,
	OPC_AND_IMM  = 5,
	OPC_OR_IMM   = 6,
	OPC_XOR_IMM  = 7,
	OPC_LB_OFF   = 8,
	OPC_SB_OFF   = 9,
	OPC_ALU_REG  = 10,
	OPC_BRANCH   = 11,
	OPC_MOV_IMM  = 12,
	OPC_LW_OFF   = 13,
	OPC_SW_OFF   = 14,
	OPC_IMM      = 15

} NANO_OPC;

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

#define OPC_IMM4(opc)   ((opc) & 0x0f)
#define OPC_OFF4(opc)   ((opc) & 0x0f)
#define OPC_IMM8(opc)   ((opc) & 0xff)

#define OPC_IMM12(opc)  ((opc) & 0xFFF)

#define SIGN_EXT(x,b)   (((x) & (b)) ? (((x) & ((b) - 1)) - (b)) : ((x) & ((b) - 1)))

/*
 *  Simulation Model
 */

typedef unsigned short NANO_INST;
typedef unsigned short NANO_SHORT;
typedef unsigned long  NANO_LONG;
typedef unsigned long  NANO_TIME;

#ifdef _NANO32
	typedef signed   long NANO_SWORD;
	typedef unsigned long NANO_WORD;
	typedef unsigned long NANO_ADDR;
	typedef __int64       NANO_DWORD;
	#define NANO_BITS	32
	#define NANO_MSB    0x80000000L
	#define NANO_SZADDR "%08x"
#else
	typedef signed   short NANO_SWORD;
	typedef unsigned short NANO_WORD;
	typedef unsigned short NANO_ADDR;
	typedef long           NANO_DWORD;
	#define NANO_SZADDR "%04x"
	#define NANO_BITS	16
	#define NANO_MSB    0x8000
#endif

#define NANO_N		0x0001
#define NANO_C		0x0002
#define NANO_V		0x0004
#define NANO_Z		0x0008

typedef struct
{
	NANO_WORD reg[16];
	NANO_ADDR pc;
	NANO_WORD temp;
	NANO_WORD prefix;
	NANO_TIME cycles;
	NANO_WORD ccr;

	NANO_ADDR breakpoint;
} NANO_CPU;

typedef enum
{
	NANO_STEP_OVER, NANO_STEP_OUT, NANO_STEP_INTO
} NANO_STEP;

void NanoReset(NANO_CPU* pCpu);

extern NANO_WORD led_out;
extern NANO_WORD sw_inp;

int MemReadWord(NANO_ADDR addr, NANO_SHORT* data);
int MemReadLong(NANO_ADDR addr, NANO_LONG* data);
int MemWriteByte(NANO_ADDR addr, NANO_SHORT data);
int MemWriteWord(NANO_ADDR addr, NANO_SHORT data);
int MemWriteLong(NANO_ADDR addr, NANO_LONG data);
void MemCopyBytes(NANO_ADDR addr, void* buf, int length);

// Implemented by Simulator
void OutWriteWord(NANO_ADDR addr, NANO_SHORT data);
NANO_SHORT InpReadWord(NANO_ADDR addr);

int NanoSimInst(NANO_CPU* p, NANO_STEP step);
int NanoDisAsm(char* line, size_t len, NANO_ADDR addr, NANO_INST opc);

extern const char szRegName[16][4];

#ifdef __cplusplus
}
#endif

#endif /* __NANOCPU_H__ */
