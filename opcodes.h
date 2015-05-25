#if !defined( OPCODES_H )
#define OPCODES_H 1

#undef OP
#define OP( x, c ) x,
enum BasicOps
{
#include "basic_opcodes.h"
};
enum EDOps
{
#include "ed_opcodes.h"
};
enum CBOps
{
#include "cb_opcodes.h"
};
#undef OP

extern int g_basicCycleCount[];
extern const char *g_basicNames[];

extern int g_edCycleCount[];
extern const char *g_edNames[];

extern int g_cbCycleCount[];
extern const char *g_cbNames[];


#endif // OPCODES_H

