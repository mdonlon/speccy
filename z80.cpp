#include <assert.h>
#include <string.h>

#include "z80.h"
#include "opcodes.h"

#include "screen.h"

//#define ASM_PRINT printf
//#define MEM_PRINT printf
//#define REG_PRINT printf
//#define DBG_PRINT printf

#define ASM_PRINT(...) (void)0
#define MEM_PRINT(...) (void)0
#define REG_PRINT(...) (void)0
#define DBG_PRINT(...) (void)0

#include "z80_system.h"
#include "z80_support.h"
#include "z80_alu.h"
#include "z80_branch.h"
#include "z80_block.h"


void ExecCB( ZState *Z )
{
	if( Z->idx != R_HL )
	{
		ReadDisp( Z );
	}

	uint8_t fullOp = ReadPC8( Z );
	uint8_t op = fullOp >> 3;
	OpcodeRegister operandReg = (OpcodeRegister)( fullOp & 7 );
	uint8_t operand;
	uint8_t carryIn, carryOut;
	bool copyOperand = false;
	uint8_t tempu8;
	uint8_t flagMask = M_S | M_3 | M_5;

	ASM_PRINT( "CB %s\n", g_cbNames[op] );

	Z->cycles -= g_cbCycleCount[op];

	if( Z->idx != R_HL )
	{
		operand = ReadIndex( Z );
		flagMask = M_S;
		if( operandReg != OP_REG_INDEX )
			copyOperand = true;
	}
	else
	{
		switch( operandReg )
		{
			case OP_REG_B: operand = Z->reg.B; break;
			case OP_REG_C: operand = Z->reg.C; break;
			case OP_REG_D: operand = Z->reg.D; break;
			case OP_REG_E: operand = Z->reg.E; break;
			case OP_REG_H: operand = Z->reg.H; break;
			case OP_REG_L: operand = Z->reg.L; break;
			case OP_REG_INDEX:
				Z->cycles -= 7;
				flagMask = M_S;
				operand = ReadIndex( Z );
				break;
			case OP_REG_A: operand = Z->reg.A; break;
		};
		copyOperand = true;
	}

	switch( op )
	{
		case RLC:
			Z->reg.F &= ~(M_N | M_H | M_C);
			carryOut = ( operand >> 7 ) & 1;
			operand <<= 1;
			operand |= carryOut;
			Z->reg.F |= carryOut << F_C;
			SetZeroSignParity( Z, operand );
			SetF35( Z, operand );
			break;

		case RRC:
			Z->reg.F &= ~(M_N | M_H | M_C);
			carryOut = ( operand ) & 1;
			operand >>= 1;
			operand |= carryOut << 7;
			Z->reg.F |= carryOut << F_C;
			SetZeroSignParity( Z, operand );
			SetF35( Z, operand );
			break;

		case RL:
			carryIn = ( Z->reg.F >> F_C ) & 1;
			Z->reg.F &= ~(M_N | M_H | M_C);
			carryOut = ( operand >> 7 ) & 1;
			operand <<= 1;
			operand |= carryIn;
			Z->reg.F |= carryOut << F_C;
			SetZeroSignParity( Z, operand );
			SetF35( Z, operand );
			break;

		case RR:
			carryIn = ( Z->reg.F >> F_C ) & 1;
			Z->reg.F &= ~(M_N | M_H | M_C);
			carryOut = ( operand ) & 1;
			operand >>= 1;
			operand |= carryIn << 7;
			Z->reg.F |= carryOut << F_C;
			SetZeroSignParity( Z, operand );
			SetF35( Z, operand );
			break;

		case SLA:
			Z->reg.F &= ~(M_N | M_H | M_C);
			carryOut = ( operand >> 7 ) & 1;
			operand <<= 1;
			Z->reg.F |= carryOut << F_C;
			SetZeroSignParity( Z, operand );
			SetF35( Z, operand );
			break;

		case SRA:
			Z->reg.F &= ~(M_N | M_H | M_C);
			carryOut = ( operand ) & 1;
			operand = (uint8_t)( ( (int8_t) operand ) >> 1 );
			Z->reg.F |= carryOut << F_C;
			SetZeroSignParity( Z, operand );
			SetF35( Z, operand );
			break;

		case SLL:
			Z->reg.F &= ~(M_N | M_H | M_C);
			carryOut = ( operand >> 7 ) & 1;
			operand <<= 1;
			operand |= 1;
			Z->reg.F |= carryOut << F_C;
			SetZeroSignParity( Z, operand );
			SetF35( Z, operand );
			break;

		case SRL:
			Z->reg.F &= ~(M_N | M_H | M_C);
			carryOut = ( operand ) & 1;
			operand >>= 1;
			Z->reg.F |= carryOut << F_C;
			SetZeroSignParity( Z, operand );
			SetF35( Z, operand );
			break;

#define BIT_TEST(x) Z->cycles += 3; copyOperand = false; BitTest( Z, (x), operand, flagMask )
		case BIT_0: BIT_TEST( 0 ); break;
		case BIT_1: BIT_TEST( 1 ); break;
		case BIT_2: BIT_TEST( 2 ); break;
		case BIT_3: BIT_TEST( 3 ); break;
		case BIT_4: BIT_TEST( 4 ); break;
		case BIT_5: BIT_TEST( 5 ); break;
		case BIT_6: BIT_TEST( 6 ); break;
		case BIT_7: BIT_TEST( 7 ); break;

		case RES_0: operand &= ~( 1 << 0 ); break;
		case RES_1: operand &= ~( 1 << 1 ); break;
		case RES_2: operand &= ~( 1 << 2 ); break;
		case RES_3: operand &= ~( 1 << 3 ); break;
		case RES_4: operand &= ~( 1 << 4 ); break;
		case RES_5: operand &= ~( 1 << 5 ); break;
		case RES_6: operand &= ~( 1 << 6 ); break;
		case RES_7: operand &= ~( 1 << 7 ); break;

		case SET_0: operand |= ( 1 << 0 ); break;
		case SET_1: operand |= ( 1 << 1 ); break;
		case SET_2: operand |= ( 1 << 2 ); break;
		case SET_3: operand |= ( 1 << 3 ); break;
		case SET_4: operand |= ( 1 << 4 ); break;
		case SET_5: operand |= ( 1 << 5 ); break;
		case SET_6: operand |= ( 1 << 6 ); break;
		case SET_7: operand |= ( 1 << 7 ); break;

		default:
			assert( 0 );
			break;
	}
	
	if( Z->idx != R_HL )
	{
		WriteIndex( Z, operand );
	}

	if( copyOperand )
	{
		switch( operandReg )
		{
			case OP_REG_B: Z->reg.B = operand; break;
			case OP_REG_C: Z->reg.C = operand; break;
			case OP_REG_D: Z->reg.D = operand; break;
			case OP_REG_E: Z->reg.E = operand; break;
			case OP_REG_H: Z->reg.H = operand; break;
			case OP_REG_L: Z->reg.L = operand; break;
			case OP_REG_INDEX: WriteIndex( Z, operand ); break;
			case OP_REG_A: Z->reg.A = operand; break;
		};
	}
}

void ExecED( ZState *Z )
{
	uint8_t op = ReadPC8( Z );
	uint8_t tempu8;
	
	ASM_PRINT( "ED %s\n", g_edNames[op] );
	
	Z->cycles -= g_edCycleCount[op];

	switch( op )
	{
		case IM_1: Z->IMODE = 1; break;
		case IM_0: Z->IMODE = 0; break;

		case LD_I_A: Z->reg.I = Z->reg.A; break;

		case ED_LD_RNN_HL: Write16( Z, ReadPC16( Z ), Z->rIdx->w ); break;
		case LD_RNN_DE: Write16( Z, ReadPC16( Z ), Z->reg.DE ); break;
		case LD_RNN_BC: Write16( Z, ReadPC16( Z ), Z->reg.BC ); break;
		case LD_RNN_SP: Write16( Z, ReadPC16( Z ), Z->reg.SP ); break;
		
		case LD_SP_RNN: Z->reg.SP = Read16( Z, ReadPC16( Z ) ); break;
		case LD_BC_RNN: Z->reg.BC = Read16( Z, ReadPC16( Z ) ); break;
		case LD_DE_RNN: Z->reg.DE = Read16( Z, ReadPC16( Z ) ); break;
		case ED_LD_HL_RNN: Z->reg.HL = Read16( Z, ReadPC16( Z ) ); break;

		case LDD: Ldd( Z ); break;
		case LDDR: if( !Ldd( Z ) ) { Z->cycles -= 5; Z->reg.PC -= 2; }; break;
		case LDI: Ldi( Z ); break;
		case LDIR: if( !Ldi( Z ) ) { Z->cycles -= 5; Z->reg.PC -= 2; }; break;

		case CPD: Cpd( Z ); break;
		case CPDR: if( !Cpd( Z ) ) { Z->cycles -= 5; Z->reg.PC -= 2; }; break;
		case CPI: Cpi( Z ); break;
		case CPIR: if( !Cpi( Z ) ) { Z->cycles -= 5; Z->reg.PC -= 2; }; break;


		case ADC_HL_BC: AdcToIndex( Z, Z->reg.B, Z->reg.C ); break;
		case ADC_HL_DE: AdcToIndex( Z, Z->reg.D, Z->reg.E ); break;
		case ADC_HL_HL: AdcToIndex( Z, Z->reg.H, Z->reg.L ); break;
		case ADC_HL_SP: AdcToIndex( Z, Z->reg.SP >> 8, Z->reg.SP & 0xff ); break;
		case SBC_HL_BC: SbcToIndex( Z, Z->reg.B, Z->reg.C ); break;
		case SBC_HL_DE: SbcToIndex( Z, Z->reg.D, Z->reg.E ); break;
		case SBC_HL_HL: SbcToIndex( Z, Z->reg.H, Z->reg.L ); break;
		case SBC_HL_SP: SbcToIndex( Z, Z->reg.SP >> 8, Z->reg.SP & 0xff ); break;

#define IN_FLAGS(x) SetZeroSignParity( Z, x ); Z->reg.F &= ~(M_N | M_H)
		case IN_A_RC: Z->reg.A = PortInC( Z ); IN_FLAGS( Z->reg.A ); break;
		case IN_B_RC: Z->reg.B = PortInC( Z ); IN_FLAGS( Z->reg.B ); break;
		case IN_C_RC: Z->reg.C = PortInC( Z ); IN_FLAGS( Z->reg.C ); break;
		case IN_D_RC: Z->reg.D = PortInC( Z ); IN_FLAGS( Z->reg.D ); break;
		case IN_E_RC: Z->reg.E = PortInC( Z ); IN_FLAGS( Z->reg.E ); break;
		case IN_F_RC: Z->reg.F = PortInC( Z ); IN_FLAGS( Z->reg.F ); break;
		case IN_H_RC: Z->reg.H = PortInC( Z ); IN_FLAGS( Z->reg.H ); break;
		case IN_L_RC: Z->reg.L = PortInC( Z ); IN_FLAGS( Z->reg.L ); break;

		case NEG: NegateA( Z ); break;

		case RRD:
			tempu8 = ReadIndex( Z );
			WriteIndex( Z, ( tempu8 >> 4 ) | ( Z->reg.A << 4 ) );
			Z->reg.A = ( Z->reg.A & 0xf0 ) | ( tempu8 & 0x0f );
			Z->reg.F &= ~( M_N | M_H );
			SetF35( Z, Z->reg.A );
			SetZeroSignParity( Z, Z->reg.A );
			break;

		case RLD:
			tempu8 = ReadIndex( Z );
			WriteIndex( Z, ( tempu8 << 4 ) | ( Z->reg.A & 0x0f ) );
			Z->reg.A = ( Z->reg.A & 0xf0 ) | ( ( tempu8 >> 4 ) & 0x0f );
			Z->reg.F &= ~( M_N | M_H );
			SetF35( Z, Z->reg.A );
			SetZeroSignParity( Z, Z->reg.A );
			break;

		case LD_A_R:
			Z->reg.A = 0;
			SetZeroSignParity( Z, Z->reg.A );
			SetF35( Z, Z->reg.A );
			break;


		default:
			printf( "Unimplemented ED opcode 0x%02x: %s\n", op, g_edNames[op] );
			Z->halted = true;
			break;
	}
}


static const char *FlagString( uint8_t f )
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


void Exec( ZState *Z )
{
	uint16_t u16Temp;
	uint8_t u8Temp;

	REG_PRINT( "A:%02x F:%s B:%02x C:%02x D:%02x E:%02x H:%02x L:%02x I:%02x R:%02x IX:%04x IY:%04x PC:%04x SP:%04x\n",
			Z->reg.A,
			FlagString( Z->reg.F ),
			Z->reg.B, Z->reg.C, Z->reg.D, Z->reg.E,
			Z->reg.H, Z->reg.L, Z->reg.I, Z->reg.R,
			Z->reg.IX, Z->reg.IY, Z->reg.PC, Z->reg.SP );
	
	uint8_t op = ReadPC8( Z );
	
	ASM_PRINT( "%s\n", g_basicNames[op] );

	Z->cycles -= g_basicCycleCount[op];

	switch( op )
	{
		case NOP: break;
		case HALT: Z->halted = true; break;
		case DI: Z->IFF0 = 0; break;
		case EI: Z->IFF0 = 1; break;

		case OR_B: Or( Z, Z->reg.B ); break;
		case OR_C: Or( Z, Z->reg.C ); break;
		case OR_D: Or( Z, Z->reg.D ); break;
		case OR_E: Or( Z, Z->reg.E ); break;
		case OR_H: Or( Z, Z->rIdx->h ); break;
		case OR_L: Or( Z, Z->rIdx->l ); break;
		case OR_RHL: ReadDisp( Z ); Or( Z, ReadIndex( Z ) ); break;
		case OR_A: Or( Z, Z->reg.A ); break;
		case OR_N: Or( Z, ReadPC8( Z ) ); break;

		case XOR_B: Xor( Z, Z->reg.B ); break;
		case XOR_C: Xor( Z, Z->reg.C ); break;
		case XOR_D: Xor( Z, Z->reg.D ); break;
		case XOR_E: Xor( Z, Z->reg.E ); break;
		case XOR_H: Xor( Z, Z->rIdx->h ); break;
		case XOR_L: Xor( Z, Z->rIdx->l ); break;
		case XOR_RHL: ReadDisp( Z ); Xor( Z, ReadIndex( Z ) ); break;
		case XOR_A: Xor( Z, Z->reg.A ); break;
		case XOR_N: Xor( Z, ReadPC8( Z ) ); break;

		case AND_B: And( Z, Z->reg.B ); break;
		case AND_C: And( Z, Z->reg.C ); break;
		case AND_D: And( Z, Z->reg.D ); break;
		case AND_E: And( Z, Z->reg.E ); break;
		case AND_H: And( Z, Z->rIdx->h ); break;
		case AND_L: And( Z, Z->rIdx->l ); break;
		case AND_RHL: ReadDisp( Z ); And( Z, ReadIndex( Z ) ); break;
		case AND_A: And( Z, Z->reg.A ); break;
		case AND_N: And( Z, ReadPC8( Z ) ); break;

		case ADD_A_B: AddA( Z, Z->reg.B ); break;
		case ADD_A_C: AddA( Z, Z->reg.C ); break;
		case ADD_A_D: AddA( Z, Z->reg.D ); break;
		case ADD_A_E: AddA( Z, Z->reg.E ); break;
		case ADD_A_H: AddA( Z, Z->rIdx->h ); break;
		case ADD_A_L: AddA( Z, Z->rIdx->l ); break;
		case ADD_A_RHL: ReadDisp( Z ); AddA( Z, ReadIndex( Z ) ); break;
		case ADD_A_A: AddA( Z, Z->reg.A ); break;
		case ADD_A_N: AddA( Z, ReadPC8( Z ) ); break;

		case ADC_A_B: AdcA( Z, Z->reg.B ); break;
		case ADC_A_C: AdcA( Z, Z->reg.C ); break;
		case ADC_A_D: AdcA( Z, Z->reg.D ); break;
		case ADC_A_E: AdcA( Z, Z->reg.E ); break;
		case ADC_A_H: AdcA( Z, Z->rIdx->h ); break;
		case ADC_A_L: AdcA( Z, Z->rIdx->l ); break;
		case ADC_A_RHL: ReadDisp( Z ); AdcA( Z, ReadIndex( Z ) ); break;
		case ADC_A_A: AdcA( Z, Z->reg.A ); break;
		case ADC_A_N: AdcA( Z, ReadPC8( Z ) ); break;

		case SUB_A_B: SubA( Z, Z->reg.B ); break;
		case SUB_A_C: SubA( Z, Z->reg.C ); break;
		case SUB_A_D: SubA( Z, Z->reg.D ); break;
		case SUB_A_E: SubA( Z, Z->reg.E ); break;
		case SUB_A_H: SubA( Z, Z->rIdx->h ); break;
		case SUB_A_L: SubA( Z, Z->rIdx->l ); break;
		case SUB_A_RHL: ReadDisp( Z ); SubA( Z, ReadIndex( Z ) ); break;
		case SUB_A_A: SubA( Z, Z->reg.A ); break;
		case SUB_A_N: SubA( Z, ReadPC8( Z ) ); break;

		case SBC_A_B: SbcA( Z, Z->reg.B ); break;
		case SBC_A_C: SbcA( Z, Z->reg.C ); break;
		case SBC_A_D: SbcA( Z, Z->reg.D ); break;
		case SBC_A_E: SbcA( Z, Z->reg.E ); break;
		case SBC_A_H: SbcA( Z, Z->rIdx->h ); break;
		case SBC_A_L: SbcA( Z, Z->rIdx->l ); break;
		case SBC_A_RHL: ReadDisp( Z ); SbcA( Z, ReadIndex( Z ) ); break;
		case SBC_A_A: SbcA( Z, Z->reg.A ); break;
		case SBC_A_N: SbcA( Z, ReadPC8( Z ) ); break;

		case INC_B: Z->reg.B = Increment8( Z, Z->reg.B ); break;
		case INC_C: Z->reg.C = Increment8( Z, Z->reg.C ); break;
		case INC_D: Z->reg.D = Increment8( Z, Z->reg.D ); break;
		case INC_E: Z->reg.E = Increment8( Z, Z->reg.E ); break;
		case INC_H: Z->rIdx->h = Increment8( Z, Z->rIdx->h ); break;
		case INC_L: Z->rIdx->l = Increment8( Z, Z->rIdx->l ); break;
		case INC_A: Z->reg.A = Increment8( Z, Z->reg.A ); break;
		case INC_RHL: ReadDisp( Z ); WriteIndex( Z, Increment8( Z, ReadIndex( Z ) ) ); break;

		case DEC_B: Z->reg.B = Decrement8( Z, Z->reg.B ); break;
		case DEC_C: Z->reg.C = Decrement8( Z, Z->reg.C ); break;
		case DEC_D: Z->reg.D = Decrement8( Z, Z->reg.D ); break;
		case DEC_E: Z->reg.E = Decrement8( Z, Z->reg.E ); break;
		case DEC_H: Z->rIdx->h = Decrement8( Z, Z->rIdx->h ); break;
		case DEC_L: Z->rIdx->l = Decrement8( Z, Z->rIdx->l ); break;
		case DEC_A: Z->reg.A = Decrement8( Z, Z->reg.A ); break;
		case DEC_RHL: ReadDisp( Z ); WriteIndex( Z, Decrement8( Z, ReadIndex( Z ) ) ); break;

		case RLCA:
			Z->reg.F &= ~( M_N | M_H | M_C );
			u8Temp = ( Z->reg.A >> 7 ) & 1;
			Z->reg.A <<= 1;
			Z->reg.A |= u8Temp;
			Z->reg.F |= u8Temp << F_C;
			SetF35( Z, Z->reg.A );
			break;

		case RRCA:
			Z->reg.F &= ~( M_N | M_H | M_C );
			u8Temp = Z->reg.A & 1;
			Z->reg.A >>= 1;
			Z->reg.A |= u8Temp << 7;
			Z->reg.F |= u8Temp << F_C;
			SetF35( Z, Z->reg.A );
			break;

		case RRA:
			u8Temp = ( Z->reg.F >> F_C ) & 1;
			Z->reg.F &= ~( M_N | M_H | M_C );
			Z->reg.F |= ( Z->reg.A & 1 ) << F_C;
			Z->reg.A >>= 1;
			Z->reg.A |= u8Temp << 7;
			SetF35( Z, Z->reg.A );
			break;

		case RLA:
			u8Temp = ( Z->reg.F >> F_C ) & 1;
			Z->reg.F &= ~( M_N | M_H | M_C );
			Z->reg.F |= ( ( Z->reg.A >> 7 ) & 1 ) << F_C;
			Z->reg.A <<= 1;
			Z->reg.A |= u8Temp;
			SetF35( Z, Z->reg.A );
			break;


		case DAA:
			if( ( Z->reg.A & 0x0f ) > 0x09 || ( ( Z->reg.F & M_H ) != 0 ) )
			{
				Z->reg.A += 0x06;
				Z->reg.F |= M_H;
			}
			else
			{
				Z->reg.F &= ~M_H;
			}

			if( Z->reg.A > 0x90 || ( ( Z->reg.F & M_C ) != 0 ) )
			{
				Z->reg.A += 0x60;
				Z->reg.F |= M_C;
			}
			else
			{
				Z->reg.F &= ~M_C;
			}
			SetZeroSignParity( Z, Z->reg.A );
			SetF35( Z, Z->reg.A );
			break;

		case CPL:
			Z->reg.A = ~Z->reg.A;
			Z->reg.F |= ( M_N | M_H );
			SetF35( Z, Z->reg.A );
			break;

		case SCF:
			Z->reg.F |= M_C;
			Z->reg.F &= ~( M_N | M_H );
			SetF35( Z, Z->reg.A );
			break;

		case CCF:
			Z->reg.F &= ~( M_H | M_N );
			Z->reg.F |= ( ( Z->reg.F >> F_C ) & 1 ) << F_H;
			Z->reg.F ^= M_C;
			SetF35( Z, Z->reg.A );
			break;

		case ADD_HL_BC: AddToIndex( Z, Z->reg.B, Z->reg.C ); break;
		case ADD_HL_DE: AddToIndex( Z, Z->reg.D, Z->reg.E ); break;
		case ADD_HL_HL: AddToIndex( Z, Z->rIdx->h, Z->rIdx->l ); break;
		case ADD_HL_SP: AddToIndex( Z, Z->reg.SP >> 8, Z->reg.SP & 0xff ); break;

		case DEC_HL: Z->rIdx->w -= 1; break;
		case DEC_BC: Z->reg.BC -= 1; break;
		case DEC_DE: Z->reg.DE -= 1; break;
		case DEC_SP: Z->reg.SP -= 1; break;

		case INC_HL: Z->rIdx->w += 1; break;
		case INC_BC: Z->reg.BC += 1; break;
		case INC_DE: Z->reg.DE += 1; break;
		case INC_SP: Z->reg.SP += 1; break;

		case LD_RNN_A: Write8( Z, ReadPC16( Z ), Z->reg.A ); break;
		case LD_RBC_A: Write8( Z, Z->reg.BC, Z->reg.A ); break;
		case LD_RDE_A: Write8( Z, Z->reg.DE, Z->reg.A ); break;

		case LD_RHL_B: ReadDisp( Z ); WriteIndex( Z, Z->reg.B ); break;
		case LD_RHL_C: ReadDisp( Z ); WriteIndex( Z, Z->reg.C ); break;
		case LD_RHL_D: ReadDisp( Z ); WriteIndex( Z, Z->reg.D ); break;
		case LD_RHL_E: ReadDisp( Z ); WriteIndex( Z, Z->reg.E ); break;
		case LD_RHL_H: ReadDisp( Z ); WriteIndex( Z, Z->reg.H ); break;
		case LD_RHL_L: ReadDisp( Z ); WriteIndex( Z, Z->reg.L ); break;
		case LD_RHL_A: ReadDisp( Z ); WriteIndex( Z, Z->reg.A ); break;
		case LD_RHL_N: ReadDisp( Z ); WriteIndex( Z, ReadPC8( Z ) ); break;

		case LD_B_RHL: ReadDisp( Z ); Z->reg.B = ReadIndex( Z ); break;
		case LD_C_RHL: ReadDisp( Z ); Z->reg.C = ReadIndex( Z ); break;
		case LD_D_RHL: ReadDisp( Z ); Z->reg.D = ReadIndex( Z ); break;
		case LD_E_RHL: ReadDisp( Z ); Z->reg.E = ReadIndex( Z ); break;
		case LD_H_RHL: ReadDisp( Z ); Z->reg.H = ReadIndex( Z ); break;
		case LD_L_RHL: ReadDisp( Z ); Z->reg.L = ReadIndex( Z ); break;
		case LD_A_RHL: ReadDisp( Z ); Z->reg.A = ReadIndex( Z ); break;
		
		case LD_A_RBC: Z->reg.A = Read8( Z, Z->reg.BC ); break;
		case LD_A_RDE: Z->reg.A = Read8( Z, Z->reg.DE ); break;
		case LD_A_RNN: Z->reg.A = Read8( Z, ReadPC16( Z ) ); break;

		case LD_SP_NN: Z->reg.SP = ReadPC16( Z ); break;
		case LD_DE_NN: Z->reg.DE = ReadPC16( Z ); break;
		case LD_BC_NN: Z->reg.BC = ReadPC16( Z ); break;
		case LD_HL_NN: Z->rIdx->w = ReadPC16( Z ); break;
		case LD_RNN_HL: Write16( Z, ReadPC16( Z ), Z->rIdx->w ); break;
		case LD_HL_RNN: Z->rIdx->w = Read16( Z, ReadPC16( Z ) ); break;
		case LD_SP_HL: Z->reg.SP = Z->rIdx->w; break;


#define LD_RR(D,S) Z->reg.D = Z->reg.S
#define LD_RH(D) Z->reg.D = Z->rIdx->h
#define LD_RL(D) Z->reg.D = Z->rIdx->l
#define LD_HR(S) Z->rIdx->h = Z->reg.S
#define LD_LR(S) Z->rIdx->l = Z->reg.S


		case LD_A_B: LD_RR( A, B ); break;
		case LD_A_C: LD_RR( A, C ); break;
		case LD_A_D: LD_RR( A, D ); break;
		case LD_A_E: LD_RR( A, E ); break;
		case LD_A_H: LD_RH( A ); break;
		case LD_A_L: LD_RL( A ); break;
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
		case LD_C_H: LD_RH( C ); break;
		case LD_C_L: LD_RL( C ); break;
		case LD_C_A: LD_RR( C, A ); break;
		case LD_D_B: LD_RR( D, B ); break;
		case LD_D_C: LD_RR( D, C ); break;
		case LD_D_D: LD_RR( D, D ); break;
		case LD_D_E: LD_RR( D, E ); break;
		case LD_D_H: LD_RH( D ); break;
		case LD_D_L: LD_RL( D ); break;
		case LD_D_A: LD_RR( D, A ); break;
		case LD_E_B: LD_RR( E, B ); break;
		case LD_E_C: LD_RR( E, C ); break;
		case LD_E_D: LD_RR( E, D ); break;
		case LD_E_E: LD_RR( E, E ); break;
		case LD_E_H: LD_RH( E ); break;
		case LD_E_L: LD_RL( E ); break;
		case LD_E_A: LD_RR( E, A ); break;
		case LD_H_B: LD_HR( B ); break;
		case LD_H_C: LD_HR( C ); break;
		case LD_H_D: LD_HR( D ); break;
		case LD_H_E: LD_HR( E ); break;
		case LD_H_H: Z->rIdx->h = Z->rIdx->h; break;
		case LD_H_L: Z->rIdx->h = Z->rIdx->l; break;
		case LD_H_A: LD_HR( A ); break;
		case LD_L_B: LD_LR( B ); break;
		case LD_L_C: LD_LR( C ); break;
		case LD_L_D: LD_LR( D ); break;
		case LD_L_E: LD_LR( E ); break;
		case LD_L_H: Z->rIdx->l = Z->rIdx->h; break;
		case LD_L_L: Z->rIdx->l = Z->rIdx->l; break;
		case LD_L_A: LD_LR( A ); break;
		
#define LD_RI(D) Z->reg.D = ReadPC8( Z )
		case LD_A_N: LD_RI( A ); break;
		case LD_B_N: LD_RI( B ); break;
		case LD_C_N: LD_RI( C ); break;
		case LD_D_N: LD_RI( D ); break;
		case LD_E_N: LD_RI( E ); break;
		case LD_H_N: Z->rIdx->h = ReadPC8( Z ); break;
		case LD_L_N: Z->rIdx->l = ReadPC8( Z ); break;

		case OUT_RN_A: PortOut( Z ); break;
		case IN_A_RN: PortIn( Z ); break;

		case CP_A: Compare( Z, Z->reg.A ); break;
		case CP_B: Compare( Z, Z->reg.B ); break;
		case CP_C: Compare( Z, Z->reg.C ); break;
		case CP_D: Compare( Z, Z->reg.D ); break;
		case CP_E: Compare( Z, Z->reg.E ); break;
		case CP_H: Compare( Z, Z->rIdx->h ); break;
		case CP_L: Compare( Z, Z->rIdx->l ); break;
		case CP_N: Compare( Z, ReadPC8( Z ) ); break;
		case CP_RHL: ReadDisp( Z ); Compare( Z, ReadIndex( Z ) ); break;

		case PUSH_BC: Push16( Z, Z->reg.BC ); break;
		case PUSH_DE: Push16( Z, Z->reg.DE ); break;
		case PUSH_HL: Push16( Z, Z->rIdx->w ); break;
		case PUSH_AF: Push16( Z, Z->reg.AF ); break;

		case POP_BC: Z->reg.BC = Pop16( Z ); break;
		case POP_DE: Z->reg.DE = Pop16( Z ); break;
		case POP_HL: Z->rIdx->w = Pop16( Z ); break;
		case POP_AF: Z->reg.AF = Pop16( Z ); break;

		case RST_00: Push16( Z, Z->reg.PC ); Z->reg.PC = 0x00; break;
		case RST_08: Push16( Z, Z->reg.PC ); Z->reg.PC = 0x08; break;
		case RST_10: Push16( Z, Z->reg.PC ); Z->reg.PC = 0x10; break;
		case RST_18: Push16( Z, Z->reg.PC ); Z->reg.PC = 0x18; break;
		case RST_20: Push16( Z, Z->reg.PC ); Z->reg.PC = 0x20; break;
		case RST_28: Push16( Z, Z->reg.PC ); Z->reg.PC = 0x28; break;
		case RST_30: Push16( Z, Z->reg.PC ); Z->reg.PC = 0x30; break;
		case RST_38: Push16( Z, Z->reg.PC ); Z->reg.PC = 0x38; break;

		case CALL_NN: Call( Z, 1 ); break;
		case CALL_C_NN: Call( Z, Z->reg.F & M_C ); break;
		case CALL_NC_NN: Call( Z, (~Z->reg.F) & M_C ); break;
		case CALL_Z_NN: Call( Z, Z->reg.F & M_Z ); break;
		case CALL_NZ_NN: Call( Z, (~Z->reg.F) & M_Z ); break;
		case CALL_M_NN: Call( Z, Z->reg.F & M_S ); break;
		case CALL_P_NN: Call( Z, (~Z->reg.F) & M_S ); break;
		case CALL_PE_NN: Call( Z, Z->reg.F & M_P ); break;
		case CALL_PO_NN: Call( Z, (~Z->reg.F) & M_P ); break;


		case RET: Return( Z, 1 ); break;
		case RET_C: Return( Z, Z->reg.F & M_C ); break;
		case RET_NC: Return( Z, (~Z->reg.F) & M_C ); break;
		case RET_Z: Return( Z, Z->reg.F & M_Z ); break;
		case RET_NZ: Return( Z, (~Z->reg.F) & M_Z ); break;
		case RET_M: Return( Z, Z->reg.F & M_S ); break;
		case RET_P: Return( Z, (~Z->reg.F) & M_S ); break;
		case RET_PE: Return( Z, Z->reg.F & M_P ); break;
		case RET_PO: Return( Z, (~Z->reg.F) & M_P ); break;


		case JP_HL: Z->reg.PC = Z->rIdx->w; break;
		case JP_NN: Jump( Z, 1 ); break;
		case JP_NZ_NN: Jump( Z, (~Z->reg.F) & M_Z ); break;
		case JP_Z_NN: Jump( Z, Z->reg.F & M_Z ); break;
		case JP_NC_NN: Jump( Z, (~Z->reg.F) & M_C ); break;
		case JP_C_NN: Jump( Z, Z->reg.F & M_C ); break;
		case JP_M_NN: Jump( Z, Z->reg.F & M_S ); break;
		case JP_P_NN: Jump( Z, (~Z->reg.F) & M_S ); break;

		case JR_N: JumpRelative( Z, 1 ); break;
		case JR_NZ_N: JumpRelative( Z, (~Z->reg.F) & M_Z ); break;
		case JR_Z_N: JumpRelative( Z, Z->reg.F & M_Z ); break;
		case JR_NC_N: JumpRelative( Z, (~Z->reg.F) & M_C ); break;
		case JR_C_N: JumpRelative( Z, Z->reg.F & M_C ); break;
		
		case DJNZ_N: Z->reg.B--; JumpRelative( Z, Z->reg.B ); break;

		case EXX: Exchange( Z ); break;
		case EX_AF_AF: u16Temp = Z->reg.AF; Z->reg.AF = Z->sreg.AF; Z->sreg.AF = u16Temp; break;
		case EX_DE_HL: u16Temp = Z->reg.DE; Z->reg.DE = Z->rIdx->w; Z->rIdx->w = u16Temp; break;
		case EX_RSP_HL:
			u16Temp = Z->rIdx->w;
			Z->rIdx->w = Read16( Z, Z->reg.SP );
			Write16( Z, Z->reg.SP, u16Temp );
			break;

		case PREFIX_CB: ExecCB( Z ); break;
		case PREFIX_ED: ExecED( Z ); break;
		case PREFIX_DD: SetIndexRegister( Z, R_IX ); Exec( Z ); break;
		case PREFIX_FD: SetIndexRegister( Z, R_IY ); Exec( Z ); break;

		default:
			printf( "Unimplemented opcode 0x%02x: %s\n", op, g_basicNames[op] );
			Z->halted = true;
			break;
	}
}

void Z80_MaskableInterrupt( ZState *Z )
{
	Z->INT = 1;
}

void Z80_NonMaskableInterrupt( ZState *Z )
{
	Z->NMI = 1;
}

void Z80_Init( ZState *Z )
{
	memset( Z, 0, sizeof( ZState ) );
}

void Z80_Reset( ZState *Z )
{
	memset( &Z->reg, 0, sizeof( Z->reg ) );
	memset( &Z->sreg, 0, sizeof( Z->sreg ) );

	Z->INT = 0;
	Z->NMI = 0;
	Z->IFF0 = 0;
}

void Z80_SnapshotResume( ZState *Z )
{
	Z->IFF0 = Z->IFF1;
	Z->reg.PC = Pop16( Z );
}

void Z80_Run( ZState *Z, int cycles )
{
	if( Z->NMI )
	{
		Z->IFF1 = Z->IFF0;
		Z->IFF0 = 0;
		Z->NMI = 0;
		Push16( Z, Z->reg.PC );
		Z->reg.PC = 0x0066;
		Z->halted = false;
	}
	else if( Z->INT && Z->IFF0 )
	{
		Z->IFF0 = Z->IFF1 = 0;
		Z->INT = 0;
		Push16( Z, Z->reg.PC );
		if( Z->IMODE == 1 )
		{
			Z->reg.PC = 0x0038;
		}
		else
		{
			Z->reg.PC = Z->reg.I << 8;
		}
		Z->halted = false;
	}

	Z->cycles += cycles;

	while( !Z->halted && Z->cycles > 0 )
	{
		SetIndexRegister( Z, R_HL );
		Exec( Z );
	}

	if( Z->halted )
		Z->cycles = 0;
}

