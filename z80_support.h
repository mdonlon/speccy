#if !defined( Z80_SUPPORT_H )
#define Z80_SUPPORT_H 1

uint8_t ReadPC8( ZState *Z )
{
	uint8_t value = Read8( Z, Z->reg.PC );
	Z->reg.PC++;
	return value;
}

uint16_t ReadPC16( ZState *Z )
{
	uint16_t value = Read16( Z, Z->reg.PC );
	Z->reg.PC += 2;
	return value;
}

void SetIndexRegister( ZState *Z, IndexRegister idx )
{
	assert( idx == R_HL || idx == R_IX || idx == R_IY );
	Z->idx = idx;
	Z->rIdx = &Z->reg.r[idx];
	Z->disp = 0;
}

uint8_t ReadIndex( ZState *Z )
{
	uint16_t addr;
	addr = Z->reg.r[Z->idx].w;
	addr += Z->disp;

	return Read8( Z, addr );
}

void WriteIndex( ZState *Z, uint8_t v )
{
	uint16_t addr;
	addr = Z->reg.r[Z->idx].w;
	addr += Z->disp;

	return Write8( Z, addr, v );
}

void ReadDisp( ZState *Z )
{
	if( Z->idx == R_HL )
		return;
	Z->cycles -= 8;
	Z->disp = (int8_t)ReadPC8( Z );
}

void Push8( ZState *Z, uint8_t v )
{
	Z->reg.SP--;
	Write8( Z, Z->reg.SP, v );
}

void Push16( ZState *Z, uint16_t v )
{
	Push8( Z, v >> 8 );
	Push8( Z, v & 0xff );
}

uint8_t Pop8( ZState *Z )
{
	uint8_t v = Read8( Z, Z->reg.SP );
	Z->reg.SP++;

	return v;
}

uint16_t Pop16( ZState *Z )
{
	uint16_t v = 0;
	v = Pop8( Z );
	v |= Pop8( Z ) << 8;

	return v;
}

void PortOut( ZState *Z )
{
	uint16_t addr = ( Z->reg.A << 8 ) | ReadPC8( Z );
	ASM_PRINT( "OUT: (%04x) <- %02x\n", addr, Z->reg.A );
	WritePort( Z, addr, Z->reg.A );
}

void PortIn( ZState *Z )
{
	uint16_t addr = ( Z->reg.A << 8 ) | ReadPC8( Z );
	Z->reg.A = ReadPort( Z, addr );
	ASM_PRINT( "IN: (%04x) -> %02x\n", addr, Z->reg.A );
}

void PortOutC( ZState *Z, uint8_t v )
{
	uint16_t port = Z->reg.BC;
	WritePort( Z, port, v );
	ASM_PRINT( "OUT: (%04x) <- %02x\n", port, v );
}

uint8_t PortInC( ZState *Z )
{
	uint16_t port = Z->reg.BC;
	uint8_t v = ReadPort( Z, port );
	ASM_PRINT( "IN: (%04x) -> %02x\n", port, v );
	return v;
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

#endif // Z80_SUPPORT_H
