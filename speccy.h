#if !defined( SPECCY_H )
#define SPECCY_H 1

enum SpeccyKey
{
	SK_SHIFT, SK_Z,   SK_X, SK_C, SK_V,
	SK_A,     SK_S,   SK_D, SK_F, SK_G,
	SK_Q,     SK_W,   SK_E, SK_R, SK_T,
	SK_1,     SK_2,   SK_3, SK_4, SK_5,
	SK_0,     SK_9,   SK_8, SK_7, SK_6,
	SK_P,     SK_O,   SK_I, SK_U, SK_Y,
	SK_ENTER, SK_L,   SK_K, SK_J, SK_H,
	SK_SPACE, SK_SYM, SK_M, SK_N, SK_B
};

struct SpeccyKeyState
{
	uint8_t row[8];
};

#define SK_ROW(sk) ((sk) >> 5)
#define SK_BIT(sk) ((sk) & 0x1f)
#define SK_MASK(sk) (1 << SK_BIT((sk)))

#endif // SPECCY_H
