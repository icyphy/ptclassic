/* err.h  edg
Version identification:
$Id$
Header for error handing routines.
*/

#define ERR_IF1(cond) \
    if (cond) { \
	return(FALSE); \
    }

#define ERR_IF2(cond, msg) \
    if (cond) { \
	ErrAdd(msg); \
	return(FALSE); \
    }

#define ERR_IF3(cond, msg, retVal) \
    if (cond) { \
	ErrAdd(msg); \
	return(retVal); \
    }

extern void ErrClear();
extern void ErrAdd();
extern char *ErrGet();
