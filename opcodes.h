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
enum FDOps
{
#include "fd_opcodes.h"
};
enum DDOps
{
#include "dd_opcodes.h"
};
#undef OP

extern int g_basicCycleCount[];
extern const char *g_basicNames[];

extern int g_edCycleCount[];
extern const char *g_edNames[];

extern int g_fdCycleCount[];
extern const char *g_fdNames[];

extern int g_ddCycleCount[];
extern const char *g_ddNames[];


#endif // OPCODES_H

