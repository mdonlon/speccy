#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "opcodes.h"

#define RAM_MASK ((uint16_t)~0x3fff)

enum Register
{
	R_B = 0,
	R_C = 1,
	R_D = 2,
	R_E = 3,
	R_H = 4,
	R_L = 5,
	R_HL_SPECIAL = 6,
	R_A = 7
};

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
};

#define ASM_PRINT printf
#define MEM_PRINT printf
#define REG_PRINT printf



/* UTILITY */
uint8_t ReadByte( ZState *Z, uint16_t address )
{
	uint8_t value = Z->mem[address];

	MEM_PRINT( "Read 0x%04x -> 0x%02x\n", address, value );

	return value;
}

void WriteByte( ZState *Z, uint16_t address, uint8_t value )
{
	MEM_PRINT( "Write 0x%04x <- 0x%02x\n", address, value );

	if( ( address & RAM_MASK ) != 0 )
		Z->mem[address] = value;
}

uint16_t GetHL( ZState *Z )
{
	return Z->reg.H << 8 | Z->reg.L;
}

void SetHL( ZState *Z, uint16_t v )
{
	Z->reg.H = (uint8_t)( v >> 8 );
	Z->reg.L = (uint8_t)v;
}

uint8_t ReadPC( ZState *Z )
{
	uint8_t value = Z->mem[Z->reg.PC];
	Z->reg.PC++;
	return value;
}

uint8_t ReadHL( ZState *Z )
{
	uint16_t addr = GetHL( Z );
	return ReadByte( Z, addr );
}

void WriteHL( ZState *Z, uint8_t v )
{
	uint16_t addr = GetHL( Z );
	return WriteByte( Z, addr, v );
}


/* Opcode Functions */
void PortOut( ZState *Z )
{
	uint8_t port = ReadPC( Z );
	ASM_PRINT( "OUT: (%02x) <- %02x\n", port, Z->reg.A );
}

void PortIn( ZState *Z )
{
	uint8_t port = ReadPC( Z );
	Z->reg.A = 0x00;
	ASM_PRINT( "IN: (%02x) -> %02x\n", port, Z->reg.A );
}

void SetParity( ZState *Z, uint8_t v )
{
	v ^= v >> 4;
	int parity = ( 0x6996 >> v ) & 1;
	Z->reg.F &= ~M_P;
	Z->reg.F |= parity << F_P;
}

void SetZero( ZState *Z, uint8_t v )
{
	if( v == 0 )
	{
		Z->reg.F |= M_Z;
	}
	else
	{
		Z->reg.F &= ~M_Z;
	}
}

void SetSign( ZState *Z, uint8_t v )
{
	Z->reg.F &= ~M_S;
	Z->reg.F |= v & M_S;
}

void SetZeroSignParity( ZState *Z, uint8_t v )
{
	Z->reg.F &= ~(M_S | M_Z | M_P);

	if( v == 0 )
		Z->reg.F |= M_Z;

	Z->reg.F |= v & M_S;

	v ^= v >> 4;
	Z->reg.F |= ( ( 0x6996 >> v ) << F_P ) & M_P;
}

void Xor( ZState *Z, uint8_t v )
{
	Z->reg.A ^= v;
	Z->reg.F &= ~( M_C | M_N | M_H ); // reset C, H, N
	SetZeroSignParity( Z, Z->reg.A );
}

void And( ZState *Z, uint8_t v )
{
	Z->reg.A &= v;
	Z->reg.F &= ~( M_C | M_N | M_H ); // reset C, H, N
	SetZeroSignParity( Z, Z->reg.A );
}


void Jump( ZState *Z, uint8_t cond )
{
	uint16_t addr = ReadPC( Z ) | ( ReadPC( Z ) << 8 );
	if( cond != 0 )
	{
		Z->reg.PC = addr;
	}
}

void JumpRelative( ZState *Z, uint8_t cond )
{
	int8_t addr = (int8_t)ReadPC( Z );
	if( cond != 0 )
	{
		Z->reg.PC = Z->reg.PC + addr;
	}
}


void Cp( ZState *Z, uint8_t v )
{
	uint8_t flag = v & ( M_3 | M_5 );

	const uint16_t r16 = (uint16_t)Z->reg.A - (uint16_t)v;
	const uint8_t r8 = (uint8_t)r16;

	flag |= M_N;
	flag |= r8 & M_S;

	if( r8 == 0 )
		flag |= M_Z;

	if( r16 & 0x100 )
		flag |= M_C;

	Z->reg.F = flag;
}

void ExecED( ZState *Z )
{
	uint8_t op = ReadPC( Z );
	
	ASM_PRINT( "ED %s\n", g_edNames[op] );

	switch( op )
	{
		case LD_I_A: Z->reg.I = Z->reg.A; break;
		default:
			printf( "Unimplemented ED opcode 0x%02x: %s\n", op, g_edNames[op] );
			break;
	}
}

void Exec( ZState *Z )
{
	REG_PRINT( "A:%02x B:%02x C:%02x D:%02x E:%02x H:%02x L:%02x F:%02x I:%02x R:%02x IX:%04x IY:%04x PC:%04x SP:%04x\n",
			Z->reg.A, Z->reg.B, Z->reg.C, Z->reg.D, Z->reg.E,
			Z->reg.H, Z->reg.L, Z->reg.F, Z->reg.I, Z->reg.R,
			Z->reg.IX, Z->reg.IY, Z->reg.PC, Z->reg.SP );

	uint8_t op = ReadPC( Z );
	
	ASM_PRINT( "%s\n", g_basicNames[op] );

	switch( op )
	{
		case NOP: break;
		case DI: printf( "DI NOT IMPLEMENTED!\n" ); break;
		case XOR_B: Xor( Z, Z->reg.B ); break;
		case XOR_C: Xor( Z, Z->reg.C ); break;
		case XOR_D: Xor( Z, Z->reg.D ); break;
		case XOR_E: Xor( Z, Z->reg.E ); break;
		case XOR_H: Xor( Z, Z->reg.H ); break;
		case XOR_L: Xor( Z, Z->reg.L ); break;
		case XOR_RHL: Xor( Z, ReadHL( Z ) ); break;
		case XOR_A: Xor( Z, Z->reg.A ); break;

		case AND_B: And( Z, Z->reg.B ); break;
		case AND_C: And( Z, Z->reg.C ); break;
		case AND_D: And( Z, Z->reg.D ); break;
		case AND_E: And( Z, Z->reg.E ); break;
		case AND_H: And( Z, Z->reg.H ); break;
		case AND_L: And( Z, Z->reg.L ); break;
		case AND_RHL: And( Z, ReadHL( Z ) ); break;
		case AND_A: And( Z, Z->reg.A ); break;


		case DEC_HL: SetHL( Z, GetHL( Z ) - 1 ); break;

		case LD_RHL_N: WriteHL( Z, ReadPC( Z ) ); break;

		case LD_DE_NN: Z->reg.E = ReadPC( Z ); Z->reg.D = ReadPC( Z ); break;
		case LD_BC_NN: Z->reg.C = ReadPC( Z ); Z->reg.B = ReadPC( Z ); break;
		case LD_HL_NN: Z->reg.L = ReadPC( Z ); Z->reg.H = ReadPC( Z ); break;

#define LD_RR(D,S) Z->reg.D = Z->reg.S
		case LD_A_B: LD_RR( A, B ); break;
		case LD_A_C: LD_RR( A, C ); break;
		case LD_A_D: LD_RR( A, D ); break;
		case LD_A_E: LD_RR( A, E ); break;
		case LD_A_H: LD_RR( A, H ); break;
		case LD_A_L: LD_RR( A, L ); break;
		case LD_A_A: LD_RR( A, A ); break;
		case LD_B_B: LD_RR( B, B ); break;
		case LD_B_C: LD_RR( B, C ); break;
		case LD_B_D: LD_RR( B, D ); break;
		case LD_B_E: LD_RR( B, E ); break;
		case LD_B_H: LD_RR( B, H ); break;
		case LD_B_L: LD_RR( B, L ); break;
		case LD_B_A: LD_RR( B, A ); break;
		case LD_C_B: LD_RR( C, B ); break;
		case LD_C_C: LD_RR( C, C ); break;
		case LD_C_D: LD_RR( C, D ); break;
		case LD_C_E: LD_RR( C, E ); break;
		case LD_C_H: LD_RR( C, H ); break;
		case LD_C_L: LD_RR( C, L ); break;
		case LD_C_A: LD_RR( C, A ); break;
		case LD_D_B: LD_RR( D, B ); break;
		case LD_D_C: LD_RR( D, C ); break;
		case LD_D_D: LD_RR( D, D ); break;
		case LD_D_E: LD_RR( D, E ); break;
		case LD_D_H: LD_RR( D, H ); break;
		case LD_D_L: LD_RR( D, L ); break;
		case LD_D_A: LD_RR( D, A ); break;
		case LD_E_B: LD_RR( E, B ); break;
		case LD_E_C: LD_RR( E, C ); break;
		case LD_E_D: LD_RR( E, D ); break;
		case LD_E_E: LD_RR( E, E ); break;
		case LD_E_H: LD_RR( E, H ); break;
		case LD_E_L: LD_RR( E, L ); break;
		case LD_E_A: LD_RR( E, A ); break;
		case LD_H_B: LD_RR( H, B ); break;
		case LD_H_C: LD_RR( H, C ); break;
		case LD_H_D: LD_RR( H, D ); break;
		case LD_H_E: LD_RR( H, E ); break;
		case LD_H_H: LD_RR( H, H ); break;
		case LD_H_L: LD_RR( H, L ); break;
		case LD_H_A: LD_RR( H, A ); break;
		case LD_L_B: LD_RR( L, B ); break;
		case LD_L_C: LD_RR( L, C ); break;
		case LD_L_D: LD_RR( L, D ); break;
		case LD_L_E: LD_RR( L, E ); break;
		case LD_L_H: LD_RR( L, H ); break;
		case LD_L_L: LD_RR( L, L ); break;
		case LD_L_A: LD_RR( L, A ); break;
		
#define LD_RI(D) Z->reg.D = ReadPC( Z )
		case LD_A_N: LD_RI( A ); break;
		case LD_B_N: LD_RI( B ); break;
		case LD_C_N: LD_RI( C ); break;
		case LD_D_N: LD_RI( D ); break;
		case LD_E_N: LD_RI( E ); break;
		case LD_H_N: LD_RI( H ); break;
		case LD_L_N: LD_RI( L ); break;

		case OUT_RN_A: PortOut( Z ); break;
		case IN_A_RN: PortIn( Z ); break;

		case CP_A: Cp( Z, Z->reg.A ); break;
		case CP_B: Cp( Z, Z->reg.B ); break;
		case CP_C: Cp( Z, Z->reg.C ); break;
		case CP_D: Cp( Z, Z->reg.D ); break;
		case CP_E: Cp( Z, Z->reg.E ); break;
		case CP_H: Cp( Z, Z->reg.H ); break;
		case CP_L: Cp( Z, Z->reg.L ); break;

		case JP_NN: Jump( Z, 1 ); break;
		case JP_NZ_NN: Jump( Z, (~Z->reg.F) & M_Z ); break;
		case JP_Z_NN: Jump( Z, Z->reg.F & M_Z ); break;
		case JP_NC_NN: Jump( Z, (~Z->reg.F) & M_C ); break;
		case JP_C_NN: Jump( Z, Z->reg.F & M_C ); break;

		case JR_N: JumpRelative( Z, 1 ); break;
		case JR_NZ_N: JumpRelative( Z, (~Z->reg.F) & M_Z ); break;
		case JR_Z_N: JumpRelative( Z, Z->reg.F & M_Z ); break;
		case JR_NC_N: JumpRelative( Z, (~Z->reg.F) & M_C ); break;
		case JR_C_N: JumpRelative( Z, Z->reg.F & M_C ); break;

		case PREFIX_ED: ExecED( Z ); break;
		default:
			printf( "Unimplemented opcode 0x%02x: %s\n", op, g_basicNames[op] );
			abort();
			break;
	}
}

void Init( ZState *Z )
{
	memset( Z, 0, sizeof( ZState ) );
	FILE *fp = fopen( "roms/48.rom", "rb" );
	if( fp == NULL )
	{
		printf( "Could not read rom file\n" );
		abort();
	}

	fread( Z->mem, 16 * 1024, 1, fp );
	fclose( fp );
}


int main( int argc, char *argv[] )
{
	ZState Z;
	Init( &Z );

	for( int i = 0; i < 3200000; i ++ )
	{
		Exec( &Z );
	}

	return 0;
}

