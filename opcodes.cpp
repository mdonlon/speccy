#include "opcodes.h"

#define OP( x, c ) c,
int g_basicCycleCount[] =
{
#include "basic_opcodes.h"
};
int g_edCycleCount[] =
{
#include "ed_opcodes.h"
};
int g_fdCycleCount[] =
{
#include "fd_opcodes.h"
};
int g_ddCycleCount[] =
{
#include "dd_opcodes.h"
};
#undef OP

#define OP(x, c) #x,
const char *g_basicNames[] =
{
#include "basic_opcodes.h"
};
const char *g_edNames[] =
{
#include "ed_opcodes.h"
};
const char *g_fdNames[] =
{
#include "fd_opcodes.h"
};
const char *g_ddNames[] =
{
#include "dd_opcodes.h"
};
#undef OP
