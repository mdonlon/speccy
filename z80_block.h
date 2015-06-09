#if !defined( Z80_BLOCK_H )
#define Z80_BLOCK_H 1

bool Ldd( ZState *Z )
{
	uint8_t v = Read8( Z, Z->reg.HL );
	Write8( Z, Z->reg.DE, v );
	Z->reg.HL -= 1;
	Z->reg.DE -= 1;
	Z->reg.BC -= 1;

	v += Z->reg.A;

	Z->reg.F &= ~( M_N | M_P | M_H | M_3 | M_5 );

	Z->reg.F |= v & M_3;
	Z->reg.F |= ( v << 4 ) & M_5;

	if( Z->reg.BC == 0 )
	{
		return true;
	}

	Z->reg.F |= M_P;

	return false;
}

bool Ldi( ZState *Z )
{
	uint8_t v = Read8( Z, Z->reg.HL );
	Write8( Z, Z->reg.DE, v );
	Z->reg.HL += 1;
	Z->reg.DE += 1;
	Z->reg.BC -= 1;

	v += Z->reg.A;

	Z->reg.F &= ~( M_N | M_P | M_H | M_3 | M_5 );

	Z->reg.F |= v & M_3;
	Z->reg.F |= ( v << 4 ) & M_5;

	if( Z->reg.BC == 0 )
	{
		return true;
	}

	Z->reg.F |= M_P;

	return false;
}

bool Cpd( ZState *Z )
{
	uint8_t v = Read8( Z, Z->reg.HL );
	Z->reg.HL -= 1;
	Z->reg.BC -= 1;

	uint8_t cflags = CompareValues( Z->reg.A, v );

	Z->reg.F &= ~( M_S | M_Z | M_P | M_H | M_3 | M_5 );
	Z->reg.F |= M_N | ( cflags & ( M_S | M_Z | M_H ) );

	uint8_t H = ( cflags >> F_H ) & 1;

	uint8_t c = Z->reg.A - v - H;
	Z->reg.F |= c & M_3;
	Z->reg.F |= ( c << 4 ) & M_5;

	if( Z->reg.BC != 0 )
	{
		Z->reg.F |= M_P;
	}

	if( Z->reg.BC || ( Z->reg.F & M_Z ) )
		return true;

	return false;
}

bool Cpi( ZState *Z )
{
	uint8_t v = Read8( Z, Z->reg.HL );
	Z->reg.HL += 1;
	Z->reg.BC -= 1;

	uint8_t cflags = CompareValues( Z->reg.A, v );

	Z->reg.F &= ~( M_S | M_Z | M_P | M_H | M_3 | M_5 );
	Z->reg.F |= M_N | ( cflags & ( M_S | M_Z | M_H ) );

	uint8_t H = ( cflags >> F_H ) & 1;

	uint8_t c = Z->reg.A - v - H;
	Z->reg.F |= c & M_3;
	Z->reg.F |= ( c << 4 ) & M_5;

	if( Z->reg.BC != 0 )
	{
		Z->reg.F |= M_P;
	}

	if( Z->reg.BC || ( Z->reg.F & M_Z ) )
		return true;

	return false;
}


#endif // Z80_BLOCK_H
