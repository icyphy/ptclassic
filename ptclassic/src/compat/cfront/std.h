// $Id$
#if defined(hppa)
#include <stdlib.h>
#include <sys/unistd.h>
#include "../../kernel/miscFuncs.h"
#define random() ((long)(rand()))
#define srandom(seed) (srand( (unsigned int)(seed))
#else
// Sun
#include <stdlib.h>
#include <sysent.h>
#include "../../kernel/miscFuncs.h"
// we could include <strings.h> too, but Sun Cfront 2.1 has an
// incorrect prototype for strcasecmp.
extern "C" int strcasecmp(const char*, const char*);
#endif

