/* nanocpu.h */

#ifndef __NANOCPU_H__
#define __NANOCPU_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define NANO_FUNC           12
#define NANO_RD             8
#define NANO_RA             4
#define NANO_RB             0
#define NANO_FUNC           12

/*
 *  Arithmetic Logic Unit
 */

typedef enum
{
    ALU_ADD   =  0,     /* ADD */
    ALU_SUB   =  1,     /* SUBtract */
    ALU_ADC   =  2,     /* Add with Carry */
    ALU_SBC   =  3,     /* SuBtract with Carry */
    ALU_AND   =  4,     /* And */
    ALU_OR    =  5,     /* Or */
    ALU_XOR   =  6,     /* eXclusive Or */
	ALU_SHIFT =  7,     /* SHIFT (right,left, rotate) */

} NANO_ALU;

/*
 *  Conditional Branch
 */
typedef enum
{
    COND_BNE,       /* Branch Not Equal */
    COND_BEQ,       /* Branch EQual */
    COND_BHI,       /* Branch HIgher */
    COND_BLS,       /* Branch Lower/Same */
    COND_BHS,       /* Branch Higher/Same */
    COND_BLO,       /* Branch LOwer */
    COND_BGT,       /* Branch if Greater Than */
    COND_BLE,       /* Branch if Less/Equal */
    COND_BGE,       /* Branch if Greater/Equal */
    COND_BLT,       /* Branch if Less Than */
    COND_BRN,       /* BRnch Never */
    COND_BRA,       /* BRanch Always */
    COND_BPL,       /* Branch if PLus */
    COND_BMI,       /* Branch if MInus */
    COND_JAL,       /* Jump And Link */
    COND_REP,       /* Repeat Instruction */
} BRANCH_COND;

/*
 *  Load/Store field(s)
 */
typedef enum
{
    OPC_BYTE = 0,
    OPC_WORD = 1,
    OPC_LONG = 2,
	OPC_SIZE_MASK = 3,

	OPC_LDST_BIT = 4
} OPC_SIZE;
	
/*
 *  4-bit "opcode" field
 */
typedef enum
{
    OPC_ADD_REG  = 0,
    OPC_SUB_REG  = 1,
	OPC_ADD_IMM  = 2,
	OPC_SUB_IMM  = 3,
	OPC_AND_IMM  = 4,
	OPC_OR_IMM   = 5,
	OPC_XOR_IMM  = 6,
	OPC_USR1_IMM = 7,
	OPC_USR2_IMM = 8,
	OPC_USR3_IMM = 9,
	OPC_ALU_REG  = 10,
	OPC_BRANCH   = 11,
	OPC_CALL     = 12,
	OPC_LDST     = 13,
	OPC_LEA_OFF  = 14,
	OPC_PREFIX   = 15

} NANO_OPC;

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
	#define NANO_BITS	16
    #define NANO_MSB    0x8000
    #define NANO_SZADDR "%04x"
#endif

#define NANO_N      0x0001
#define NANO_C      0x0002
#define NANO_V      0x0004
#define NANO_Z      0x0008

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
int MemWriteWord(NANO_ADDR addr, NANO_SHORT data);
int MemWriteLong(NANO_ADDR addr, NANO_LONG data);
void MemCopyBytes(NANO_ADDR addr, void* buf, int length);

int NanoSimInst(NANO_CPU* p, NANO_STEP step);
int NanoDisAsm(const NANO_ADDR addr, char* line);

#ifdef __cplusplus
}
#endif

#endif /* __NANOCPU_H__ */
