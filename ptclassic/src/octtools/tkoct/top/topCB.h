#if !defined(_TOPCB_H_)
#define _TOPCB_H_ 1
/* 
    RCS: $Header$
    Author: Kennard White

    Copyright (c) 1990-93 The Regents of the University of California.
    All rights reserved.  See the file "./copyright.h" for full copyright
    notice, limitation of liability, and disclaimer of warranty provisions.
#include "copyright.h"				COPYRIGHTENDKEY
*/

typedef TOPStatus (TOPCallbackFunc)( TOPPtr cbClosure, TOPPtr userData);
typedef struct _TOPCallback	TOPCallback,  *TOPCallbackHandle;
typedef struct _TOPCallbacks	TOPCallbacks, *TOPCallbacksHandle;

#define TOP_CB_OK	0
#define TOP_CB_REMOVE	1

/*
 *	topCB.c 
 */
extern TOPCallbacks*	topCallbacksCreate( char *listName);
extern void		topCallbacksDestroy( TOPCallbacks *pCBS);
extern TOPCallback*	topCallbacksFind( TOPCallbacks *pCBS, 
			  TOPCallbackFunc *cbF, TOPPtr cbC);
extern TOPCallback*	topCallbacksAdd( TOPCallbacks *pCBS, 
			  TOPCallbackFunc *cbF, TOPPtr cbC);
extern TOPStatus	topCallbacksRemove( TOPCallbacks *pCBS,
			  TOPCallbackFunc *cbF, TOPPtr cbC);
extern TOPStatus	topCallbacksCall( TOPCallbacks *pCBS, TOPPtr userData);
/*
 *	topIdleCB.c
 */
extern void		topIdleCallbacksInit();
extern void		topIdleCallbacksUninit();
extern TOPCallback*	topIdleCallbacksAdd( TOPCallbackFunc *cbF, TOPPtr cbC);
extern TOPStatus	topIdleCallbacksRemove( 
			  TOPCallbackFunc *cbF, TOPPtr cbC);
extern TOPStatus	topIdleCallbacksCall( TOPPtr userData);
#endif /* _TOPCB_H_ */
