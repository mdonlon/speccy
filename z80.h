#if !defined( Z80_H )
#define Z80_H 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define RAM_MASK ((uint16_t)~0x3fff)

enum Flag
{
	F_C = 0,
	F_N = 1,
	F_P = 2,
	F_V = 2,
	F_3 = 3,
	F_H = 4,
	F_5 = 5,
	F_Z = 6,
	F_S = 7,

	M_C = 1 << F_C,
	M_N = 1 << F_N,
	M_P = 1 << F_P,
	M_V = 1 << F_V,
	M_3 = 1 << F_3,
	M_H = 1 << F_H,
	M_5 = 1 << F_5,
	M_Z = 1 << F_Z,
	M_S = 1 << F_S,
};

enum OpcodeRegister
{
	OP_REG_B = 0,
	OP_REG_C = 1,
	OP_REG_D = 2,
	OP_REG_E = 3,
	OP_REG_H = 4,
	OP_REG_L = 5,
	OP_REG_INDEX = 6,
	OP_REG_A = 7,
};

#if defined( BIG_ENDIAN )
#define PAIR(a,b,p) union { struct { uint8_t b; uint8_t a; }; uint16_t p; }
#else
#define PAIR(a,b,p) union { struct { uint8_t a; uint8_t b; }; uint16_t p; }
#endif // defined( LITTLE_ENDIAN )

typedef PAIR(h,l,w) Register;

enum IndexRegister
{
	R_HL = 0,
	R_IX = 1,
	R_IY = 2,

	R_BC = 3,
	R_DE = 4,
	R_AF = 5
};

struct RegisterSet
{
	union
	{
		struct
		{
			PAIR( H, L, HL );
			PAIR( IXH, IXL, IX );
			PAIR( IYH, IYL, IY );
			PAIR( B, C, BC );
			PAIR( D, E, DE );
			PAIR( A, F, AF );
		};
		Register r[6];
	};

	uint8_t I;
	uint8_t R;

	// 16-bit
	uint16_t PC;
	uint16_t SP;
};

struct ZState;

struct ZPeripheral
{
	uint16_t mask;
	uint16_t address;

	uint8_t (*Read)( ZState *, uint16_t );
	void (*Write)( ZState *, uint16_t, uint8_t );
};

enum ZMemoryType
{
	MEM_ROM,
	MEM_RAM,
};

struct ZMemory
{
	ZMemoryType type;
	uint16_t base;
	uint32_t size;
	uint8_t *ptr;
};

struct ZState
{
	RegisterSet reg;
	RegisterSet sreg;

	uint8_t IFF0:1;
	uint8_t IFF1:1;
	uint8_t NMI:1;
	uint8_t INT:1;
	uint8_t IMODE:2;


	int8_t disp;
	IndexRegister idx;
	Register *rIdx;

	bool halted;

	int peripheralCount;
	ZPeripheral peripheral[8];

	int memoryCount;
	ZMemory memory[8];
};

void Z80_Reset( ZState *Z );
void Z80_Init( ZState *Z );
void Z80_Run( ZState *Z, int cycles );
void Z80_MaskableInterrupt( ZState *Z );
void Z80_NonMaskableInterrupt( ZState *Z );


#endif // !defined( Z80_H )

