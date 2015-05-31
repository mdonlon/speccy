#if !defined( SCREEN_H )
#define SCREEN_H 1

struct ZState;
struct SpeccyKeyState;

bool Screen_Init();
void Screen_Shutdown();

bool Screen_Continue();
void Screen_PollInput( SpeccyKeyState *keyState );
void Screen_UpdateScanline( int scanline, const uint8_t *mem );
void Screen_UpdateFrame();

#endif // SCREEN_H

