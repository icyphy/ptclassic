/* 
    Author: Kennard White

Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY
*/
#if !defined(lint) && !defined(SABER)
static char SccsId[] = "$Id$";
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
