// $Id$
#if defined(hppa)
#include <stdlib.h>
#include "../../kernel/miscFuncs.h"
#else
// Sun
#include <stdlib.h>
#include <sysent.h>
#include "../../kernel/miscFuncs.h"
// we could include <strings.h> too, but Sun Cfront 2.1 has an
// incorrect prototype for strcasecmp.
extern "C" int strcasecmp(const char*, const char*);
#endif

