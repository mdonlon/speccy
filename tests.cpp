#include "z80.h"
#include "opcodes.h"

#include <assert.h>

#define EXECUTE( Z, ... ) do { uint8_t ops[] = { __VA_ARGS__ }; Z80_Reset( &Z ); memcpy( Z.mem, ops, sizeof( ops ) ); Z80_Run( &Z, -1 ); } while( 0 )
#define EXPECT_FLAG_SET( Z, f ) assert( Z.reg.F & ( 1 << f ) )
#define EXPECT_FLAG_RESET( Z, f ) assert( ( Z.reg.F & ( 1 << f ) ) != ( 1 << f ) )

void XorParityTest()
{
	ZState Z;
	EXECUTE( Z, XOR_N, 0x01, HALT );
	EXPECT_FLAG_RESET( Z, F_P );
	EXECUTE( Z, XOR_N, 0x03, HALT );
	EXPECT_FLAG_SET( Z, F_P );
}

