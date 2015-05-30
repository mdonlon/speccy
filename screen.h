#if !defined( SCREEN_H )
#define SCREEN_H 1

struct ZState;
struct SpeccyKeyState;

bool Screen_Init();
void Screen_Shutdown();

bool Screen_Continue();
void Screen_PollInput( SpeccyKeyState *keyState );
void Screen_UpdateScanline( ZState *Z, int scanline );
void Screen_UpdateFrame( uint8_t *mem );

#endif // SCREEN_H

