#if !defined( Z80_BRANCH_H )
#define Z80_BRANCH_H 1

void Jump( ZState *Z, uint8_t cond )
{
	uint16_t addr = ReadPC16( Z );
	if( cond != 0 )
	{
		Z->cycles -= 9;
		Z->reg.PC = addr;
	}
}

void Call( ZState *Z, uint8_t cond )
{
	uint16_t addr = ReadPC16( Z );
	if( cond != 0 )
	{
		Z->cycles -= 16;
		Push16( Z, Z->reg.PC );
		Z->reg.PC = addr;
	}
}

void Return( ZState *Z, uint8_t cond )
{
	if( cond != 0 )
	{
		Z->cycles -= 6;
		uint16_t addr = Pop16( Z );
		Z->reg.PC = addr;
	}
}


void JumpRelative( ZState *Z, uint8_t cond )
{
	int8_t addr = (int8_t)ReadPC8( Z );
	if( cond != 0 )
	{
		Z->cycles -= 5; // 12 cycles total, 7 from opcode, 5 from branch
		Z->reg.PC = Z->reg.PC + addr;
	}
}


#endif // Z80_BRANCH_H
