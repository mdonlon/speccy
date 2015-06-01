#include "SDL.h"

#include "screen.h"
#include "z80.h"
#include "speccy.h"

static SDL_Window *s_window;
static SDL_Surface *s_surface;
static bool s_quitRequested = false;

const static SDL_Color s_colors[2][8] =
{
	{
		{ 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0xcd },
		{ 0xcd, 0x00, 0x00 },
		{ 0xcd, 0x00, 0xcd },
		{ 0x00, 0xcd, 0x00 },
		{ 0x00, 0xcd, 0xcd },
		{ 0xcd, 0xcd, 0x00 },
		{ 0xcd, 0xcd, 0xcd },
	},
	{
		{ 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0xff },
		{ 0xff, 0x00, 0x00 },
		{ 0xff, 0x00, 0xff },
		{ 0x00, 0xff, 0x00 },
		{ 0x00, 0xff, 0xff },
		{ 0xff, 0xff, 0x00 },
		{ 0xff, 0xff, 0xff },
	},
};

#define SCALE 2

bool Screen_Init()
{
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL_Init failed: %s\n", SDL_GetError() );
		return false;
	}

	s_window = SDL_CreateWindow( "Speccy", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH * SCALE, SCREEN_HEIGHT * SCALE, SDL_WINDOW_SHOWN );
	s_surface = SDL_CreateRGBSurface( 0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0 );
	s_quitRequested = false;

	return true;
}

void Screen_Shutdown()
{
	SDL_FreeSurface( s_surface );
	SDL_DestroyWindow( s_window );
	SDL_Quit();
}

bool Screen_Continue()
{
	return !s_quitRequested;
}

struct KeyMapping
{
	int key;
	SpeccyKey sk;
};

static const KeyMapping s_keymap[] =
{
	{ SDLK_RSHIFT, SK_SHIFT },
	{ SDLK_LSHIFT, SK_SHIFT },
	{ SDLK_LCTRL, SK_SYM },
	{ SDLK_RCTRL, SK_SYM },
	{ SDLK_RETURN, SK_ENTER },
	{ SDLK_SPACE, SK_SPACE },
	{ SDLK_a, SK_A },
	{ SDLK_b, SK_B },
	{ SDLK_c, SK_C },
	{ SDLK_d, SK_D },
	{ SDLK_e, SK_E },
	{ SDLK_f, SK_F },
	{ SDLK_g, SK_G },
	{ SDLK_h, SK_H },
	{ SDLK_i, SK_I },
	{ SDLK_j, SK_J },
	{ SDLK_k, SK_K },
	{ SDLK_l, SK_L },
	{ SDLK_m, SK_M },
	{ SDLK_n, SK_N },
	{ SDLK_o, SK_O },
	{ SDLK_p, SK_P },
	{ SDLK_q, SK_Q },
	{ SDLK_r, SK_R },
	{ SDLK_s, SK_S },
	{ SDLK_t, SK_T },
	{ SDLK_u, SK_U },
	{ SDLK_v, SK_V },
	{ SDLK_w, SK_W },
	{ SDLK_x, SK_X },
	{ SDLK_y, SK_Y },
	{ SDLK_z, SK_Z },
	{ SDLK_0, SK_0 },
	{ SDLK_1, SK_1 },
	{ SDLK_2, SK_2 },
	{ SDLK_3, SK_3 },
	{ SDLK_4, SK_4 },
	{ SDLK_5, SK_5 },
	{ SDLK_6, SK_6 },
	{ SDLK_7, SK_7 },
	{ SDLK_8, SK_8 },
	{ SDLK_9, SK_9 },
};

static const int s_keymapSize = sizeof( s_keymap ) / sizeof( s_keymap[0] );

static void ProcessKey( int key, bool down, SpeccyKeyState *keyState )
{
	for( int i = 0; i < s_keymapSize; i++ )
	{
		if( key == s_keymap[i].key )
		{
			const SpeccyKey sk = s_keymap[i].sk;
			if( down )
			{
				keyState->row[SK_ROW(sk)] &= ~SK_MASK(sk);
			}
			else
			{
				keyState->row[SK_ROW(sk)] |= SK_MASK(sk);
			}
			return;
		}
	}
}


void Screen_PollInput( SpeccyKeyState *keyState )
{
	SDL_Event e;
	while( SDL_PollEvent( &e ) != 0 )
	{
		switch( e.type )
		{
			case SDL_QUIT:
				s_quitRequested = true;
				break;

			case SDL_KEYDOWN:
				ProcessKey( e.key.keysym.sym, true, keyState );
				break;

			case SDL_KEYUP:
				ProcessKey( e.key.keysym.sym, false, keyState );
				break;

			default:
				break;
		}
	}
}

void Screen_UpdateScanline( uint8_t frame, int scanline, const uint8_t *mem, uint8_t borderIndex )
{
	if( scanline < VBLANK_HEIGHT )
		return;

	const int y = scanline - VBLANK_HEIGHT;
	const int sy = scanline - ( VBLANK_HEIGHT + TOP_BORDER_HEIGHT );
			
	const SDL_Color *borderColor = &s_colors[0][borderIndex];
	uint32_t border = SDL_MapRGB( s_surface->format, borderColor->r, borderColor->g, borderColor->b );
	
	SDL_LockSurface( s_surface );
	
	uint32_t *dest = ((uint32_t *)s_surface->pixels) + ( y * s_surface->w );

	for( int x = 0; x < BORDER_WIDTH; x++ )
	{
		*dest = border;
		dest++;
	}

	if( sy < 0 || sy >= PIXEL_HEIGHT )
	{
		for( int x = 0; x < PIXEL_WIDTH; x++ )
		{
			*dest = border;
			dest++;
		}
	}
	else
	{
		const int srcY = ( ( sy >> 3 ) & 7 ) | ( ( sy & 7 ) << 3 ) | ( sy & ( 3 << 6 ) );
		const uint8_t *src = mem + ( srcY * 32 );
		const uint8_t *attr = mem + ( PIXEL_HEIGHT * 32 ) + ( ( sy >> 3 ) * 32 );

		for( int x = 0; x < PIXEL_WIDTH; x+=8 )
		{
			int8_t flash = ( frame << 4 ) & *attr;
			flash >>= 7;

			uint8_t attrInk = ( *attr ^ flash ) & 0x7;
			uint8_t attrPaper = ( ( *attr ^ flash ) >> 3 ) & 0x7;
			uint8_t attrBright = ( *attr >> 6 ) & 0x1;
			uint8_t attrFlash = ( *attr >> 7 ) & 0x1;

			const SDL_Color *inkColor = &s_colors[attrBright][attrInk];
			const SDL_Color *paperColor = &s_colors[attrBright][attrPaper];

			uint32_t ink = SDL_MapRGB( s_surface->format, inkColor->r, inkColor->g, inkColor->b );
			uint32_t paper = SDL_MapRGB( s_surface->format, paperColor->r, paperColor->g, paperColor->b );
			for( int b = 7; b >= 0; b-- )
			{
				if( *src & ( 1 << b ) )
				{
					*dest = ink;
				}
				else
				{
					*dest = paper;
				}
				dest++;
			}
			src++;
			attr++;
		}
	}

	for( int x = 0; x < BORDER_WIDTH; x++ )
	{
		*dest = border;
		dest++;
	}


	SDL_UnlockSurface( s_surface );

}

void Screen_UpdateFrame()
{
	SDL_Surface *windowSurface = SDL_GetWindowSurface( s_window );
	SDL_BlitScaled( s_surface, NULL, windowSurface, NULL );
	SDL_UpdateWindowSurface( s_window );
}

