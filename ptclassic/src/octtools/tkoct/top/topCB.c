/* 
    Author: Kennard White

    Copyright (c) 1990-93 The Regents of the University of California.
    All rights reserved.  See the file "./copyright.h" for full copyright
    notice, limitation of liability, and disclaimer of warranty provisions.
#include "copyright.h"				COPYRIGHTENDKEY
*/
#if !defined(lint) && !defined(SABER)
static char rcsid[] = "$Header$";
#endif

#include "topFixup.h"
#include "topStd.h"
#include "topMem.h"
#include "topDlList.h"
#include "topCB.h"


/* typedef in topCB.h */
struct _TOPCallback {
    TOPCallbackFunc*	Func;
    TOPPtr		Closure;
};

typedef TOPMask TOPCallbacksFlags;
#define TOPCBSFlgs_CallActive	TOP_BIT(0)
#define TOPCBSFlgs_Destroy	TOP_BIT(1)

/* typedef in topCB.h */
struct _TOPCallbacks {
    char*		Name;
    TOPCallbacksFlags	Flags;
    DLListHandle	CBList;
};


static DLListHandle _TopCallbacksList = NULL;

TOPCallbacks*
topCallbacksCreate( char *listName) {
    TOPCallbacks	*pCBS;

    if ( _TopCallbacksList == NULL ) {
	_TopCallbacksList = dlListCreate("TOPCallbacks", sizeof(TOPCallbacks));
    }
    pCBS = (TOPCallbacks*) dlListCreateAtTail( _TopCallbacksList);
    pCBS->Name = listName;
    pCBS->Flags = 0;
    pCBS->CBList = dlListCreate( listName, sizeof(TOPCallback));
    return pCBS;
}

void
topCallbacksDestroy( TOPCallbacks *pCBS) {
    if ( pCBS->Flags & TOPCBSFlgs_CallActive ) {
	/* defer the destroy */
	pCBS->Flags |= TOPCBSFlgs_Destroy;
	return;
    }
    dlListDestroy( pCBS->CBList, NULL, NULL);
    pCBS->CBList = NULL;
    dlListElemDestroy( _TopCallbacksList, pCBS);
    pCBS = NULL;
}

TOPCallback*
topCallbacksFind( TOPCallbacks *pCBS, TOPCallbackFunc *cbF, TOPPtr cbC) {
    if ( cbF == NULL )
	return NULL;
    
    DLLIST_LOOP_FORW_BEGIN(pCBS->CBList, TOPCallback*, pCB) {
	if ( pCB->Func == cbF && (cbC == NULL || pCB->Closure == cbC) )
	    return pCB;
    } DLLIST_LOOP_FORW_END();
    return NULL;
}

TOPCallback*
topCallbacksAdd( TOPCallbacks *pCBS, TOPCallbackFunc *cbF, TOPPtr cbC) {
    TOPCallback		*pCB;

    if ( cbF == NULL )
	return NULL;
    /*
     * if ( (pCB=topCallbacksFind( pCBS, cbF, cbC)) != NULL )
     *	return pCB;
     * We have problems here.  If we try and add the same callback
     * while inside of that callback, and TOP_CB_REMOVE is returned,
     * then we can drop the callback completely.  Dont do it for now.
     */
    pCB = (TOPCallback*) dlListCreateAtTail( pCBS->CBList);
    pCB->Func = cbF;
    pCB->Closure = cbC;
    return pCB;
}

TOPStatus
topCallbacksRemove( TOPCallbacks *pCBS, TOPCallbackFunc *cbF, TOPPtr cbC) {
    TOPCallback		*pCB;

    if ( (pCB=topCallbacksFind( pCBS, cbF, cbC)) == NULL )
	return -1;
    if ( pCBS->Flags & TOPCBSFlgs_CallActive ) {
	/* can't do it now */
	pCB->Func = NULL;
	return 0;
    }
    dlListElemDestroy( pCBS->CBList, pCB);
    return 0;
}

TOPStatus
topCallbacksCall( TOPCallbacks *pCBS, TOPPtr userData) {
    TOPStatus		cbRet = 0;
    int			callCnt = 0;

    if ( pCBS->Flags & TOPCBSFlgs_CallActive ) {
	topProgMsg( "recursive topCallbacksCall, callbacks ``%s''", pCBS->Name);
	return -1;
    }
    pCBS->Flags |= TOPCBSFlgs_CallActive;

    DLLIST_LOOP_FORW_BEGIN(pCBS->CBList, TOPCallback*, pCB) {
        if ( pCBS->Flags & TOPCBSFlgs_Destroy )
	    break;

	if ( pCB->Func == NULL ) {
	    /* its safe to kill it now */
    	    dlListElemDestroy( pCBS->CBList, pCB);
	    continue;
	}
	++callCnt;
	cbRet = (*(pCB->Func))( pCB->Closure, userData);
	if ( cbRet == TOP_CB_REMOVE ) {
	    pCB->Func = NULL;
	    /* its safe to kill it now */
    	    dlListElemDestroy( pCBS->CBList, pCB);
	}
    } DLLIST_LOOP_FORW_END();

    pCBS->Flags &= ~TOPCBSFlgs_CallActive;
    if ( pCBS->Flags & TOPCBSFlgs_Destroy ) {
	topCallbacksDestroy( pCBS);
	return callCnt;
    }
    return callCnt;
}
