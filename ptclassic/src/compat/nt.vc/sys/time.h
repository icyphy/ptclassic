#include <winsock2.h>
#ifdef PT_NT4VC
// FIXME!! NT does not have SIGALRM!
#define SIGALRM 4
#endif

