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

	ZState Z;
	ZPeripheral ula;
	ula.mask = 0x0001;
	ula.address = 0x0000;
	ula.Read = ULARead;
	Z80_InitSpectrum( &Z );
	Z.peripherals[0] = ula;
	Z.peripheralCount = 1;

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

		Screen_UpdateFrame( &Z );
		Z80_MaskableInterrupt( &Z );
	}

	Screen_Shutdown();

	return 0;
}

