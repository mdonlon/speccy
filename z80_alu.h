#if !defined( Z80_ALU_H )
#define Z80_ALU_H 1

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

void SetF35( ZState *Z, uint8_t v )
{
	Z->reg.F &= ~(M_3 | M_5);
	Z->reg.F |= v & ( M_3 | M_5 );
}


void Or( ZState *Z, uint8_t v )
{
	Z->reg.A |= v;
	Z->reg.F &= ~( M_C | M_N | M_H ); // reset C, H, N
	SetZeroSignParity( Z, Z->reg.A );
	SetF35( Z, Z->reg.A );
}

void Xor( ZState *Z, uint8_t v )
{
	Z->reg.A ^= v;
	Z->reg.F &= ~( M_C | M_N | M_H ); // reset C, H, N
	SetZeroSignParity( Z, Z->reg.A );
	SetF35( Z, Z->reg.A );
}

void And( ZState *Z, uint8_t v )
{
	Z->reg.A &= v;
	Z->reg.F &= ~( M_C | M_N ); // reset C, N
	Z->reg.F |= M_H;
	SetF35( Z, Z->reg.A );
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
	uint8_t hOut = ( cIns >> 4 ) & 1;
	uint8_t overflow = ( cIns >> 7 ) ^ cOut;

	*flags = 0;
	*flags |= ( result & ( M_S | M_3 | M_5 ) ) | ( cOut << F_C ) | ( overflow << F_V );
	if( result == 0 )
		*flags |= M_Z;

	if( hOut )
		*flags |= M_H;

	return result;
}


void AdcA( ZState *Z, uint8_t b )
{
	Z->reg.A = AddWithCarry8( Z->reg.A, b, &Z->reg.F );
}

void AddA( ZState *Z, uint8_t b )
{
	Z->reg.F &= ~M_C;
	Z->reg.A = AddWithCarry8( Z->reg.A, b, &Z->reg.F );
}

void SbcA( ZState *Z, uint8_t b )
{
	Z->reg.F ^= M_C;
	Z->reg.A = AddWithCarry8( Z->reg.A, ~b, &Z->reg.F );
	Z->reg.F ^= M_C | M_H;
	Z->reg.F |= M_N;
}

void SubA( ZState *Z, uint8_t b )
{
	Z->reg.F |= M_C;
	Z->reg.A = AddWithCarry8( Z->reg.A, ~b, &Z->reg.F );
	Z->reg.F ^= M_C | M_H;
	Z->reg.F |= M_N;
}

void NegateA( ZState *Z )
{
	Z->reg.F |= M_C;
	Z->reg.A = AddWithCarry8( 0, ~Z->reg.A, &Z->reg.F );
	Z->reg.F ^= M_C | M_H;
	Z->reg.F |= M_N;
}

void AddToIndex( ZState *Z, uint8_t high, uint8_t low )
{
	uint8_t flags = 0;
	Z->rIdx->l = AddWithCarry8( Z->rIdx->l, low, &flags );
	Z->rIdx->h = AddWithCarry8( Z->rIdx->h, high, &flags );

	flags &= M_C | M_H | M_3 | M_5;
	Z->reg.F &= ~( M_C | M_H | M_3 | M_5 | M_N );
	Z->reg.F |= flags;
}

void AdcToIndex( ZState *Z, uint8_t high, uint8_t low )
{
	uint8_t flags = Z->reg.F;
	Z->rIdx->l = AddWithCarry8( Z->rIdx->l, low, &flags );
	Z->rIdx->h = AddWithCarry8( Z->rIdx->h, high, &flags );

	if( Z->rIdx->l != 0 )
		flags &= ~M_Z;

	flags &= ~M_N;

	Z->reg.F = flags;
}

void SbcToIndex( ZState *Z, uint8_t high, uint8_t low )
{
	uint8_t flags = Z->reg.F ^ M_C;

	//uint16_t result = Z->rIdx->w - ( ( high << 8 ) | low );
	Z->rIdx->l = AddWithCarry8( Z->rIdx->l, ~low, &flags );
	Z->rIdx->h = AddWithCarry8( Z->rIdx->h, ~high, &flags );

	//assert( result == Z->rIdx->w );

	flags ^= M_C | M_H;
	flags |= M_N;

	if( Z->rIdx->l != 0 )
		flags &= ~M_Z;

	Z->reg.F = flags;
}

uint8_t Increment8( ZState *Z, uint8_t v )
{
	uint8_t flags = Z->reg.F & ~M_C;
	v = AddWithCarry8( v, 1, &flags );
	Z->reg.F = ( Z->reg.F & M_C ) | ( flags & ~M_C );
	Z->reg.F &= ~M_N;
	return v;
}

uint8_t Decrement8( ZState *Z, uint8_t v )
{
	uint8_t flags = Z->reg.F | M_C;
	v = AddWithCarry8( v, ~1, &flags );
	Z->reg.F = ( Z->reg.F & M_C ) | ( flags & ~M_C );
	Z->reg.F |= M_N;
	Z->reg.F ^= M_H;
	return v;
}

uint8_t CompareValues( uint8_t a, uint8_t b )
{
	uint8_t flags = M_C;
	AddWithCarry8( a, ~b, &flags );
	flags ^= M_C | M_H;
	flags &= ~( M_3 | M_5 );
	flags |= b & ( M_3 | M_5 );
	flags |= M_N;
	return flags;
}


void Compare( ZState *Z, uint8_t v )
{
	Z->reg.F = CompareValues( Z->reg.A, v );
}

void BitTest( ZState *Z, int bit, uint8_t v, uint8_t flagMask )
{

	Z->reg.F |= M_H;
	Z->reg.F &= ~( M_N | M_Z | M_P | flagMask );

	uint8_t mask = 1 << bit;

	Z->reg.F |= flagMask & mask & v;

	if( ( v & mask ) == 0 )
		Z->reg.F |= M_Z | M_P;
}
	
#endif // Z80_ALU_H

