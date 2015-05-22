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

struct Registers
{
	// 8-bit
	uint8_t B;
	uint8_t C;
	uint8_t D;
	uint8_t E;
	uint8_t H;
	uint8_t L;
	uint8_t A;

	uint8_t F;
	uint8_t I;
	uint8_t R;

	// 16-bit
	uint16_t IX;
	uint16_t IY;
	uint16_t PC;
	uint16_t SP;
};


struct ZState
{
	uint8_t mem[64 * 1024];
	Registers reg;

	bool halted;
};

void Z80_Reset( ZState *Z );
void Z80_InitSpectrum( ZState *Z );
void Z80_Run( ZState *Z, int cycles );


#endif // !defined( Z80_H )

