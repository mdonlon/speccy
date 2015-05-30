#include "z80.h"
#include "screen.h"
#include "speccy.h"

SpeccyKeyState s_keyState;

uint8_t ULARead( ZState *Z, uint16_t addr )
{
	for( int i = 0; i < 8; i++ )
	{
		if( ( addr & ( 0x0100 << i ) ) == 0 )
			return s_keyState.row[i] & 0x1f;
	}
	return 0x00;
}

int main( int argc, char *argv[] )
{
	Screen_Init();

	uint8_t rom[16 * 1024];
	uint8_t ram[48 * 1024];

	FILE *fp = fopen( "roms/48.rom", "rb" );
	if( fp == NULL )
	{
		printf( "Could not read rom file\n" );
		abort();
	}

	fread( rom, sizeof( rom ), 1, fp );
	fclose( fp );

	ZState Z;
	Z80_Init( &Z );

	Z.memory[0].base = 0x0000;
	Z.memory[0].size = 0x4000;
	Z.memory[0].type = MEM_ROM;
	Z.memory[0].ptr = rom;

	Z.memory[1].base = 0x4000;
	Z.memory[1].size = 0xc000;
	Z.memory[1].type = MEM_RAM;
	Z.memory[1].ptr = ram;

	Z.memoryCount = 2;

	Z.peripheral[0].mask = 0x0001;
	Z.peripheral[0].address = 0x0000;
	Z.peripheral[0].Read = ULARead;
	Z.peripheralCount = 1;

	Z80_Reset( &Z );

	memset( &s_keyState, 0xff, sizeof( s_keyState ) );

	while( Screen_Continue() )
	{
		Screen_PollInput( &s_keyState );
		int scanline = 0;

		for( int scanline = 0; scanline < 312; scanline++ )
		{
			Screen_UpdateScanline( &Z, scanline );
			Z80_Run( &Z, 224 );
		}

		Screen_UpdateFrame( ram );
		Z80_MaskableInterrupt( &Z );
	}

	Screen_Shutdown();

	return 0;
}

