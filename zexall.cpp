#include "z80.h"

#include "opcodes.h"

uint8_t ram[64 * 1024];

void Out( ZState *Z, uint16_t addr, uint8_t value )
{
	switch( Z->reg.C )
	{
		case 2:
			printf( "%c", Z->reg.E );
			break;

		case 9:
			{
				char *p = (char *)( Z->memory[0].ptr + Z->reg.DE );
				while( *p != '$' )
				{
					printf( "%c", *p );
					p++;
				}
				break;
			}
	}
}

int main( int argc, char *argv[] )
{
	ZState Z;

	memset( ram, 0, sizeof( ram ) );

	const char *romName = "roms/zexall.com";

	if( argc == 2 )
	{
		romName = argv[1];
	}

	FILE *fp = fopen( romName, "rb" );
	fseek( fp, 0, SEEK_END );
	int size = ftell( fp );
	fseek( fp, 0, SEEK_SET );
	fread( ram + 0x100, size, 1, fp );
	fclose( fp );


	Z80_Init( &Z );

	Z.peripheral[0].mask = 0x0;
	Z.peripheral[0].address = 0x0;
	Z.peripheral[0].Write = Out;
	Z.peripheralCount = 1;

	Z.memory[0].base = 0x0000;
	Z.memory[0].size = 0x10000;
	Z.memory[0].type = MEM_RAM;
	Z.memory[0].ptr = ram;
	Z.memoryCount = 1;

	ram[5] = OUT_RN_A;
	ram[6] = 0xff;
	ram[7] = RET;

	Z80_Reset( &Z );

	Z.reg.PC = 0x100;

	while( true )
		Z80_Run( &Z, 10000 );

	return 0;
}

