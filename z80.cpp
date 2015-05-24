#include <assert.h>

#include "z80.h"
#include "opcodes.h"

#define ASM_PRINT printf
#define MEM_PRINT printf
#define REG_PRINT printf



/* UTILITY */
const char *FlagString( uint8_t f )
{
	static char str[9];
	str[0] = f & M_C ? 'C' : '-';
	str[1] = f & M_N ? 'B' : '-';
	str[2] = f & M_V ? 'V' : '-';
	str[3] = f & M_3 ? '3' : '-';
	str[4] = f & M_H ? 'H' : '-';
	str[5] = f & M_5 ? '5' : '-';
	str[6] = f & M_Z ? 'Z' : '-';
	str[7] = f & M_S ? 'S' : '-';
	str[8] = 0;

	return str;
}

uint8_t Read8( ZState *Z, uint16_t address )
{
	uint8_t value = Z->mem[address];

	MEM_PRINT( "Read 0x%04x -> 0x%02x\n", address, value );

	return value;
}

void Write8( ZState *Z, uint16_t address, uint8_t value )
{
	MEM_PRINT( "Write 0x%04x <- 0x%02x\n", address, value );

	if( ( address & RAM_MASK ) != 0 )
		Z->mem[address] = value;
}

uint16_t Read16( ZState *Z, uint16_t address )
{
	uint16_t value = Read8( Z, address ) | ( Read8( Z, address + 1 ) << 8 );
	return value;
}

void Write16( ZState *Z, uint16_t address, uint16_t value )
{
	Write8( Z, address, value & 0xff );
	Write8( Z, address + 1, value >> 8 );
}


uint8_t ReadPC8( ZState *Z )
{
	uint8_t value = Z->mem[Z->reg.PC];
	Z->reg.PC++;
	return value;
}

uint8_t ReadPC16( ZState *Z )
{
	uint16_t value = Z->mem[Z->reg.PC] | ( Z->mem[Z->reg.PC + 1] << 8 );
	Z->reg.PC += 2;
	return value;
}


uint8_t ReadHL( ZState *Z )
{
	return Read8( Z, Z->reg.HL );
}

void WriteHL( ZState *Z, uint8_t v )
{
	return Write8( Z, Z->reg.HL, v );
}


/* Opcode Functions */
void PortOut( ZState *Z )
{
	uint8_t port = ReadPC8( Z );
	ASM_PRINT( "OUT: (%02x) <- %02x\n", port, Z->reg.A );
}

void PortIn( ZState *Z )
{
	uint8_t port = ReadPC8( Z );
	Z->reg.A = 0x00;
	ASM_PRINT( "IN: (%02x) -> %02x\n", port, Z->reg.A );
}

void SetParity( ZState *Z, uint8_t v )
{
	v ^= v >> 4;
	v &= 0xf;
	uint8_t parity = ( ~0x6996 >> v ) & 1;
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
	v &= 0xf;
	Z->reg.F |= ( ( ~0x6996 >> v ) << F_P ) & M_P;
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

uint8_t AddWithCarry8( uint8_t a, uint8_t b, uint8_t *flags )
{
	uint8_t result;
	uint8_t cOut;
	
	if( *flags & M_C )
	{
		cOut = ( a >= ( 0xff - b ) );
		result = a + b + 1;
	}
	else
	{
		cOut = ( a > ( 0xff - b ) );
		result = a + b;
	}

	uint8_t cIns = a ^ result ^ b;
	uint8_t overflow = ( cIns >> 7 ) ^ cOut;

	*flags &= ~( M_V | M_C | M_Z | M_S );
	*flags |= ( result & M_S ) | ( cOut << M_C ) | ( overflow << M_V );
	if( result == 0 )
		*flags |= M_Z;

	return result;
}


void AdcA( ZState *Z, uint8_t b )
{
	Z->reg.A = AddWithCarry8( Z->reg.A, b, &Z->reg.F );
	Z->reg.F &= ~M_N;
}

void AddA( ZState *Z, uint8_t b )
{
	Z->reg.F &= ~( M_N | M_C );
	Z->reg.A = AddWithCarry8( Z->reg.A, b, &Z->reg.F );
}

void SbcA( ZState *Z, uint8_t b )
{
	Z->reg.F ^= M_C;
	Z->reg.A = AddWithCarry8( Z->reg.A, ~b, &Z->reg.F );
	Z->reg.F ^= M_C;
	Z->reg.F |= M_N;
}

void SubA( ZState *Z, uint8_t b )
{
	Z->reg.F |= ( M_N | M_C );
	Z->reg.A = AddWithCarry8( Z->reg.A, ~b, &Z->reg.F );
}

void AddHL( ZState *Z, uint8_t high, uint8_t low )
{
	uint8_t flags = 0;
	Z->reg.L = AddWithCarry8( Z->reg.L, low, &flags );
	Z->reg.H = AddWithCarry8( Z->reg.H, high, &flags );

	flags &= M_C | M_H;
	Z->reg.F &= ~( M_C | M_H );
	Z->reg.F |= flags;
}

void AdcHL( ZState *Z, uint8_t high, uint8_t low )
{
	uint8_t flags = Z->reg.F;
	Z->reg.L = AddWithCarry8( Z->reg.L, low, &flags );
	Z->reg.H = AddWithCarry8( Z->reg.H, high, &flags );

	if( Z->reg.L != 0 )
		flags &= ~M_Z;

	flags &= ~M_N;

	Z->reg.F = flags;
}

void SbcHL( ZState *Z, uint8_t high, uint8_t low )
{
	uint8_t flags = Z->reg.F ^ M_C;
	Z->reg.L = AddWithCarry8( Z->reg.L, ~low, &flags );
	Z->reg.H = AddWithCarry8( Z->reg.H, ~high, &flags );

	flags ^= M_C;
	flags |= M_N;

	if( Z->reg.L != 0 )
		flags &= ~M_Z;

	Z->reg.F = flags;
}

void Jump( ZState *Z, uint8_t cond )
{
	uint16_t addr = ReadPC8( Z ) | ( ReadPC8( Z ) << 8 );
	if( cond != 0 )
	{
		Z->reg.PC = addr;
	}
}

void JumpRelative( ZState *Z, uint8_t cond )
{
	int8_t addr = (int8_t)ReadPC8( Z );
	if( cond != 0 )
	{
		Z->reg.PC = Z->reg.PC + addr;
	}
}


void Compare( ZState *Z, uint8_t v )
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

void Exchange( ZState *Z )
{
	RegisterSet r;

	memcpy( &r, &Z->reg, sizeof( RegisterSet ) );

	Z->reg.BC = Z->sreg.BC;
	Z->reg.DE = Z->sreg.DE;
	Z->reg.HL = Z->sreg.HL;

	Z->sreg.BC = r.BC;
	Z->sreg.DE = r.DE;
	Z->sreg.HL = r.HL;
}

void ExecED( ZState *Z )
{
	uint8_t op = ReadPC8( Z );
	
	ASM_PRINT( "ED %s\n", g_edNames[op] );

	switch( op )
	{
		case LD_I_A: Z->reg.I = Z->reg.A; break;

		case ED_LD_RNN_HL: Write16( Z, ReadPC16( Z ), Z->reg.HL ); break;
		case LD_RNN_DE: Write16( Z, ReadPC16( Z ), Z->reg.DE ); break;
		case LD_RNN_BC: Write16( Z, ReadPC16( Z ), Z->reg.BC ); break;

		case ADC_HL_BC: AdcHL( Z, Z->reg.B, Z->reg.C ); break;
		case ADC_HL_DE: AdcHL( Z, Z->reg.D, Z->reg.E ); break;
		case ADC_HL_HL: AdcHL( Z, Z->reg.H, Z->reg.L ); break;
		case ADC_HL_SP: AdcHL( Z, Z->reg.SP >> 8, Z->reg.SP & 0xff ); break;
		case SBC_HL_BC: SbcHL( Z, Z->reg.B, Z->reg.C ); break;
		case SBC_HL_DE: SbcHL( Z, Z->reg.D, Z->reg.E ); break;
		case SBC_HL_HL: SbcHL( Z, Z->reg.H, Z->reg.L ); break;
		case SBC_HL_SP: SbcHL( Z, Z->reg.SP >> 8, Z->reg.SP & 0xff ); break;

		default:
			printf( "Unimplemented ED opcode 0x%02x: %s\n", op, g_edNames[op] );
			Z->halted = true;
			break;
	}
}

void Exec( ZState *Z )
{
	REG_PRINT( "A:%02x F:%s B:%02x C:%02x D:%02x E:%02x H:%02x L:%02x I:%02x R:%02x IX:%04x IY:%04x PC:%04x SP:%04x\n",
			Z->reg.A,
			FlagString( Z->reg.F ),
			Z->reg.B, Z->reg.C, Z->reg.D, Z->reg.E,
			Z->reg.H, Z->reg.L, Z->reg.I, Z->reg.R,
			Z->reg.IX, Z->reg.IY, Z->reg.PC, Z->reg.SP );

	uint8_t op = ReadPC8( Z );
	
	ASM_PRINT( "%s\n", g_basicNames[op] );

	switch( op )
	{
		case NOP: break;
		case HALT: Z->halted = true; break;
		case DI: printf( "DI NOT IMPLEMENTED!\n" ); break;
		case XOR_B: Xor( Z, Z->reg.B ); break;
		case XOR_C: Xor( Z, Z->reg.C ); break;
		case XOR_D: Xor( Z, Z->reg.D ); break;
		case XOR_E: Xor( Z, Z->reg.E ); break;
		case XOR_H: Xor( Z, Z->reg.H ); break;
		case XOR_L: Xor( Z, Z->reg.L ); break;
		case XOR_RHL: Xor( Z, ReadHL( Z ) ); break;
		case XOR_A: Xor( Z, Z->reg.A ); break;
		case XOR_N: Xor( Z, ReadPC8( Z ) ); break;

		case AND_B: And( Z, Z->reg.B ); break;
		case AND_C: And( Z, Z->reg.C ); break;
		case AND_D: And( Z, Z->reg.D ); break;
		case AND_E: And( Z, Z->reg.E ); break;
		case AND_H: And( Z, Z->reg.H ); break;
		case AND_L: And( Z, Z->reg.L ); break;
		case AND_RHL: And( Z, ReadHL( Z ) ); break;
		case AND_A: And( Z, Z->reg.A ); break;
		case AND_N: And( Z, ReadPC8( Z ) ); break;

		case ADD_A_B: AddA( Z, Z->reg.B ); break;
		case ADD_A_C: AddA( Z, Z->reg.C ); break;
		case ADD_A_D: AddA( Z, Z->reg.D ); break;
		case ADD_A_E: AddA( Z, Z->reg.E ); break;
		case ADD_A_H: AddA( Z, Z->reg.H ); break;
		case ADD_A_L: AddA( Z, Z->reg.L ); break;
		case ADD_A_RHL: AddA( Z, ReadHL( Z ) ); break;
		case ADD_A_A: AddA( Z, Z->reg.A ); break;
		case ADD_A_N: AddA( Z, ReadPC8( Z ) ); break;

		case ADC_A_B: AdcA( Z, Z->reg.B ); break;
		case ADC_A_C: AdcA( Z, Z->reg.C ); break;
		case ADC_A_D: AdcA( Z, Z->reg.D ); break;
		case ADC_A_E: AdcA( Z, Z->reg.E ); break;
		case ADC_A_H: AdcA( Z, Z->reg.H ); break;
		case ADC_A_L: AdcA( Z, Z->reg.L ); break;
		case ADC_A_RHL: AdcA( Z, ReadHL( Z ) ); break;
		case ADC_A_A: AdcA( Z, Z->reg.A ); break;
		case ADC_A_N: AdcA( Z, ReadPC8( Z ) ); break;

		case SUB_A_B: SubA( Z, Z->reg.B ); break;
		case SUB_A_C: SubA( Z, Z->reg.C ); break;
		case SUB_A_D: SubA( Z, Z->reg.D ); break;
		case SUB_A_E: SubA( Z, Z->reg.E ); break;
		case SUB_A_H: SubA( Z, Z->reg.H ); break;
		case SUB_A_L: SubA( Z, Z->reg.L ); break;
		case SUB_A_RHL: SubA( Z, ReadHL( Z ) ); break;
		case SUB_A_A: SubA( Z, Z->reg.A ); break;
		case SUB_A_N: SubA( Z, ReadPC8( Z ) ); break;

		case SBC_A_B: SbcA( Z, Z->reg.B ); break;
		case SBC_A_C: SbcA( Z, Z->reg.C ); break;
		case SBC_A_D: SbcA( Z, Z->reg.D ); break;
		case SBC_A_E: SbcA( Z, Z->reg.E ); break;
		case SBC_A_H: SbcA( Z, Z->reg.H ); break;
		case SBC_A_L: SbcA( Z, Z->reg.L ); break;
		case SBC_A_RHL: SbcA( Z, ReadHL( Z ) ); break;
		case SBC_A_A: SbcA( Z, Z->reg.A ); break;
		case SBC_A_N: SbcA( Z, ReadPC8( Z ) ); break;

		case ADD_HL_BC: AddHL( Z, Z->reg.B, Z->reg.C ); break;
		case ADD_HL_DE: AddHL( Z, Z->reg.D, Z->reg.E ); break;
		case ADD_HL_HL: AddHL( Z, Z->reg.H, Z->reg.L ); break;
		case ADD_HL_SP: AddHL( Z, Z->reg.SP >> 8, Z->reg.SP & 0xff ); break;

		case DEC_HL: Z->reg.HL -= 1; break;
		case INC_HL: Z->reg.HL += 1; break;

		case LD_RHL_N: Write8( Z, Z->reg.HL, ReadPC8( Z ) ); break;

		case LD_DE_NN: Z->reg.DE = ReadPC16( Z ); break;
		case LD_BC_NN: Z->reg.BC = ReadPC16( Z ); break;
		case LD_HL_NN: Z->reg.HL = ReadPC16( Z ); break;
		case LD_RNN_HL: Write16( Z, ReadPC16( Z ), Z->reg.HL ); break;

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
		
#define LD_RI(D) Z->reg.D = ReadPC8( Z )
		case LD_A_N: LD_RI( A ); break;
		case LD_B_N: LD_RI( B ); break;
		case LD_C_N: LD_RI( C ); break;
		case LD_D_N: LD_RI( D ); break;
		case LD_E_N: LD_RI( E ); break;
		case LD_H_N: LD_RI( H ); break;
		case LD_L_N: LD_RI( L ); break;

		case OUT_RN_A: PortOut( Z ); break;
		case IN_A_RN: PortIn( Z ); break;

		case CP_A: Compare( Z, Z->reg.A ); break;
		case CP_B: Compare( Z, Z->reg.B ); break;
		case CP_C: Compare( Z, Z->reg.C ); break;
		case CP_D: Compare( Z, Z->reg.D ); break;
		case CP_E: Compare( Z, Z->reg.E ); break;
		case CP_H: Compare( Z, Z->reg.H ); break;
		case CP_L: Compare( Z, Z->reg.L ); break;

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

		case EXX: Exchange( Z ); break;

		case PREFIX_ED: ExecED( Z ); break;
		default:
			printf( "Unimplemented opcode 0x%02x: %s\n", op, g_basicNames[op] );
			Z->halted = true;
			break;
	}
}

void Z80_Reset( ZState *Z )
{
	memset( Z, 0, sizeof( ZState ) );
}

void Z80_InitSpectrum( ZState *Z )
{
	Z80_Reset( Z );

	FILE *fp = fopen( "roms/48.rom", "rb" );
	if( fp == NULL )
	{
		printf( "Could not read rom file\n" );
		abort();
	}

	fread( Z->mem, 16 * 1024, 1, fp );
	fclose( fp );
}

void Z80_Run( ZState *Z, int cycles )
{
	int cycleCount = 0;
	while( !Z->halted && ( cycles < 0 || cycles > cycleCount ) )
	{
		Exec( Z );
		cycleCount++;
	}
}

int main( int argc, char *argv[] )
{
	RegisterSet a;

	a.H = 0xff;
	a.L = 0x00;

	assert( a.HL == 0xff00 );

	ZState Z;
	Z80_InitSpectrum( &Z );

	Z80_Run( &Z, -1 );

	return 0;
}

