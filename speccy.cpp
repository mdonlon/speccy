#include "z80.h"
#include "screen.h"
#include "speccy.h"

static SpeccyKeyState s_keyState;
static uint8_t s_ula;

uint8_t ULARead( ZState *Z, uint16_t addr )
{
	for( int i = 0; i < 8; i++ )
	{
		if( ( addr & ( 0x0100 << i ) ) == 0 )
			return s_keyState.row[i] & 0x1f;
	}
	return 0x00;
}

void ULAWrite( ZState *Z, uint16_t addr, uint8_t value )
{
	s_ula = value;
}

void ReadSNA( ZState *Z, const char *name, uint8_t *ram )
{
	FILE *fp = fopen( name, "rb" );
	fread( &Z->reg.I, 1, 1, fp );
	fread( &Z->sreg.HL, 2, 1, fp );
	fread( &Z->sreg.DE, 2, 1, fp );
	fread( &Z->sreg.BC, 2, 1, fp );
	fread( &Z->sreg.AF, 2, 1, fp );

	fread( &Z->reg.HL, 2, 1, fp );
	fread( &Z->reg.DE, 2, 1, fp );
	fread( &Z->reg.BC, 2, 1, fp );
	fread( &Z->reg.IY, 2, 1, fp );
	fread( &Z->reg.IX, 2, 1, fp );

	uint8_t iff;
	fread( &iff, 1, 1, fp );
	Z->IFF0 = iff & 1 ? 1 : 0;
	Z->IFF1 = iff & 2 ? 1 : 0;
	Z->NMI = 0;
	Z->INT = 0;

	fread( &Z->reg.R, 1, 1, fp );
	fread( &Z->reg.AF, 2, 1, fp );
	fread( &Z->reg.SP, 2, 1, fp );
	
	fread( &iff, 1, 1, fp );

	Z->IMODE = iff;

	fread( &iff, 1, 1, fp );

	fread( ram, 49152, 1, fp );

	fclose( fp );
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
	Z.peripheral[0].Write = ULAWrite;
	Z.peripheralCount = 1;

	Z80_Reset( &Z );

	ReadSNA( &Z, "roms/jetpac.sna", ram );
	Z80_SnapshotResume( &Z );

	memset( &s_keyState, 0xff, sizeof( s_keyState ) );

	uint8_t frame = 0;

	while( Screen_Continue() )
	{
		Screen_PollInput( &s_keyState );
		int scanline = 0;

		for( int scanline = 0; scanline < SCREEN_HEIGHT + VBLANK_HEIGHT; scanline++ )
		{
			Screen_UpdateScanline( frame, scanline, ram, s_ula & 0x7 );
			Z80_Run( &Z, 224 );
		}
		frame++;
		Screen_UpdateFrame();
		Z80_MaskableInterrupt( &Z );
	}

	Screen_Shutdown();

	return 0;
}

