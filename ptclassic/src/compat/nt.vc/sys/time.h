#include <winsock2.h>
#ifdef PT_NT4VC
/* FIXME!! NT does not have SIGALRM! */
#define SIGALRM 4
#endif

/* src/kernel/TimeVal.cc uses timercmp */
#define	timercmp(tvp, uvp, cmp) \
	/* CSTYLED */ \
	((tvp)->tv_sec cmp (uvp)->tv_sec || \
	((tvp)->tv_sec == (uvp)->tv_sec && \
	/* CSTYLED */ \
	(tvp)->tv_usec cmp (uvp)->tv_usec))
