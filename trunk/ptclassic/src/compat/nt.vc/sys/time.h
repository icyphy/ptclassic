/* Dummy time.h include file for Microsoft VC++ 5.0 */
/* @(#)time.h	1.6 01/05/99 */ 
/* DERealTime.h includes Clock.h, which includes TimeVal.h, which
 * includes time.h (this file), which includes winsock2.h, which 
 * includes windef.h, which has the nasty line:
 *    typedef FLOAT *PFLOAT;
 * This causes no end of trouble, since src/kernel/dataType.h has:
 *    extern const DataType INT, FLOAT, STRING, COMPLEX, FIX, ANYTYPE;
 * The solution is to redefine FLOAT and INT with before including
 * winsock2.h and then undefining them.
 * The real fix would be to change dataType.h so that we use PTFLOAT etc.
 */
#define FLOAT WINFLOAT
#define INT WININT
#include <winsock2.h>
#undef FLOAT
#undef INT
#undef far /* Redefine far to fix problems compiling DERealTimeStar */
#define far ptfar
#ifdef PT_NT4VC
/* FIXME!! NT does not have SIGALRM! */
#define SIGALRM 4
#endif
