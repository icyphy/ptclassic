/* 
    Author: Kennard White

    Copyright (c) 1990-93 The Regents of the University of California.
    All rights reserved.  See the file "./copyright.h" for full copyright
    notice, limitation of liability, and disclaimer of warranty provisions.
#include "copyright.h"				COPYRIGHTENDKEY
*/
#if !defined(lint) && !defined(SABER)
static char SccsId[] = "$Id$";
#endif
/**
**/

#include "topFixup.h"
#include "topStd.h"
#include "topCB.h"

static TOPCallbacksHandle _TopIdleCallbacks = NULL;
static TOPLogical _TopHaveWorkB = TOP_FALSE;

void
topIdleCallbacksInit() {
    if ( _TopIdleCallbacks == NULL )
	_TopIdleCallbacks = topCallbacksCreate( "IdleCallbacks");
}

void
topIdleCallbacksUninit() {
    topCallbacksDestroy( _TopIdleCallbacks);
    _TopIdleCallbacks = NULL;
}

TOPCallback*
topIdleCallbacksAdd( TOPCallbackFunc *cbF, TOPPtr cbC) {
    if ( _TopIdleCallbacks == NULL ) topIdleCallbacksInit();
    _TopHaveWorkB = TOP_TRUE;
    return topCallbacksAdd( _TopIdleCallbacks, cbF, cbC);
}

TOPStatus
topIdleCallbacksRemove( TOPCallbackFunc *cbF, TOPPtr cbC) {
    if ( _TopIdleCallbacks == NULL ) topIdleCallbacksInit();
    return topCallbacksRemove( _TopIdleCallbacks, cbF, cbC);
}

TOPStatus
topIdleCallbacksCall( TOPPtr userData) {
    TOPStatus	callCnt;
    if ( ! _TopHaveWorkB )
	return 0;
    if ( _TopIdleCallbacks == NULL ) topIdleCallbacksInit();
    callCnt = topCallbacksCall( _TopIdleCallbacks, userData);
    _TopHaveWorkB = callCnt;
    return callCnt;
}
