#include "copyright.h"

#define PANIC(str)	((void) fprintf(stderr, "%s: %s at %s:%d\n",	\
			    optProgName, (str), __FILE__, __LINE__), exit(-1))
#define OCT_ASSERT(ret)	if ((ret) < OCT_OK) {				\
			    ((void) fprintf(stderr,			\
				"%s: unexpected oct error (%s) at %s:%d\n", \
				optProgName, octErrorString(),		\
				__FILE__, __LINE__));			\
			    exit(-1);					\
			}

#define UPDATE		1
#define PRINT		2
#define STRICT_DOWN	4

struct useFlags {
    octId lowId, highId;
    char *usedArray;
};
