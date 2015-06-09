#if !defined( Z80_SYSTEM_H )
#define Z80_SYSTEM_H 1

uint8_t Read8( ZState *Z, uint16_t address )
{
	uint8_t value;
	for( int i = 0; i < Z->memoryCount; i++ )
	{
		if( Z->memory[i].base > address )
			continue;

		uint16_t ofs = address - Z->memory[i].base;

		if( ofs >= Z->memory[i].size )
			continue;

		value = Z->memory[i].ptr[ofs];
		break;
	}

	MEM_PRINT( "Read 0x%04x -> 0x%02x\n", address, value );

	return value;
}

void Write8( ZState *Z, uint16_t address, uint8_t value )
{
	MEM_PRINT( "Write 0x%04x <- 0x%02x\n", address, value );
	for( int i = 0; i < Z->memoryCount; i++ )
	{
		if( Z->memory[i].type == MEM_ROM )
			continue;

		if( Z->memory[i].base > address )
			continue;

		uint16_t ofs = address - Z->memory[i].base;

		if( ofs >= Z->memory[i].size )
			continue;

		Z->memory[i].ptr[ofs] = value;
		break;
	}
}

uint16_t Read16( ZState *Z, uint16_t address )
{
	uint16_t value = Read8( Z, address ) | ( ((uint16_t)Read8( Z, address + 1 )) << 8 );
	return value;
}

void Write16( ZState *Z, uint16_t address, uint16_t value )
{
	Write8( Z, address, value & 0xff );
	Write8( Z, address + 1, value >> 8 );
}

uint8_t ReadPort( ZState *Z, uint16_t addr )
{
	for( int i = 0; i < Z->peripheralCount; i++ )
	{
		if( Z->peripheral[i].Read == NULL )
			continue;

		if( ( addr & Z->peripheral[i].mask ) == Z->peripheral[i].address )
			return Z->peripheral[i].Read( Z, addr );
	}

	return 0x00;
}

void WritePort( ZState *Z, uint16_t addr, uint8_t value )
{
	for( int i = 0; i < Z->peripheralCount; i++ )
	{
		if( Z->peripheral[i].Write == NULL )
			continue;

		if( ( addr & Z->peripheral[i].mask ) == Z->peripheral[i].address )
			return Z->peripheral[i].Write( Z, addr, value );
	}
}


#endif // Z80_SYSTEM_H
