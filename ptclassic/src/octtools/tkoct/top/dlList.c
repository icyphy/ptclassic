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

#include "topFixup.h"
#include "topStd.h"

#include "topMem.h"
#include "topDlList.h"


static MEMHeapHandle _DlListHeap = NULL;

void
dlListClear(pList, elemDestructor, userData)
    DLList		*pList;
    DLElemDestructor	*elemDestructor;
    TOPPtr		userData;
{
    DLMAC_LOOP_FORW_BEGIN(&pList->head, DLElem*, pElem) {
	if ( elemDestructor != NULL ) {
	    (*elemDestructor)(pList, DL_ELEM_TO_DATAPTR(pElem, -sizeof(DLElem)),
	      userData);
	}
	memHeapObFree(pList->heap, (TOPPtr)pElem);
    } DLMAC_LOOP_FORW_END();
    DL_INIT_HEAD(&pList->head);
}

TOPStatus
dlListConstructor(pList, name, objectSize)
    DLList		*pList;
    char		*name;
    int			objectSize;
{
    DL_INIT_HEAD(&pList->head);
    pList->heap = memPublicHeapCreate(name, objectSize+sizeof(DLElem));
    if (pList->heap == NULL) {
        return -1;
    }
    return 0;
}

void
dlListDestructor(pList, elemDestructor, userData)
    DLList		*pList;
    DLElemDestructor	*elemDestructor;
    TOPPtr		userData;
{
    dlListClear(pList, elemDestructor, userData);
    DL_CLEAR(&pList->head);
    memHeapDestroy(pList->heap);
    pList->heap = NULL;
}

DLList*
dlListCreate(name, objectSize)
    char		*name;
    int			objectSize;
{
    DLList		*pList;

    if (_DlListHeap == NULL) {
	_DlListHeap = memPublicHeapCreate("DLList", sizeof(DLList));
	if (_DlListHeap == NULL)
	    return NULL;
    }
    pList = (DLList*) memHeapObAlloc(_DlListHeap);
    if (pList == NULL)
	return NULL;
    if (dlListConstructor(pList, name, objectSize) < 0)
	return NULL;
    return pList;
}

void
dlListDestroy(pList, elemDestructor, userData)
    DLList		*pList;
    DLElemDestructor	*elemDestructor;
    TOPPtr		userData;
{
    dlListDestructor(pList, elemDestructor, userData);
    memHeapObFree(_DlListHeap, (TOPPtr)pList);
}
    
TOPPtr
dlListElemCreate(pList)
    DLList		*pList;
{
    DLElem		*pElem;

    pElem = (DLElem*) memHeapObAlloc(pList->heap);
    if (pElem == NULL) {
	return NULL;
    }
    DL_CLEAR(pElem);
    return DL_ELEM_TO_DATAPTR(pElem, -sizeof(DLElem));
}

TOPStatus
dlListElemDestroy(pList, pCur)
    DLList		*pList;
    TOPPtr		pCur;
{
    DLElem		*pCurElem = DL_DATAPTR_TO_ELEM(pCur, -sizeof(DLElem));

    if (pCurElem->next != NULL) {
    	DL_REMOVE(pCurElem);
    	DL_CLEAR(pCurElem);
    }
    return memHeapObFree(pList->heap, (TOPPtr)pCurElem);
}

/*
 * Hopefully, (dlListEmptyB() == TOP_TRUE) <==> (dlListSize() == 0)
 */
TOPLogical
dlListEmptyB(pList)
    DLList		*pList;
{
    return (pList->head.next == &pList->head);
}

int
dlListSize(pList)
    DLList		*pList;
{
    DLElem		*pElem;
    int			cnt = 0;

    /*
     * Cant use normal DLMAC_LOOP here, b/c we dont want to
     * actually reference the data, just count it.
     */
    for ( pElem=pList->head.next; pElem != &pList->head; pElem=pElem->next) {
	++cnt;
    }
    return cnt;
}

TOPLogical
dlListElemValidB(pList, pCur)
    DLList		*pList;
    TOPPtr		pCur;
{
    DLElem		*pCurElem = DL_DATAPTR_TO_ELEM(pCur, -sizeof(DLElem));

    DLMAC_LOOP_FORW_BEGIN(&pList->head, DLElem*, pElem) {
	if (pElem == pCurElem)
	    return TOP_TRUE;
    } DLMAC_LOOP_FORW_END();
    return TOP_FALSE;
}

TOPPtr
dlListFind(pList, findCB, key)
    DLList		*pList;
    DLFindFunc		*findCB;
    TOPPtr		key;
{
    TOPPtr		pData;

    DLMAC_LOOP_FORW_BEGIN(&pList->head, DLElem*, pElem) {
        pData = DL_ELEM_TO_DATAPTR(pElem, -sizeof(DLElem));
	if ( (*(findCB))( pData, key) == 0 )
	    return pData;
    } DLMAC_LOOP_FORW_END();
    return NULL;
}

TOPPtr
dlListGetNth(pList, n)
    DLList		*pList;
    int			n;
{
    DLElem		*pElem;
    int			cnt = n;

    if ( cnt < 0 )
	return NULL;
    for ( pElem=pList->head.next; pElem != &pList->head; pElem=pElem->next) {
	if ( cnt-- == 0 )
	    return DL_ELEM_TO_DATAPTR(pElem, -sizeof(DLElem));
    }
    return NULL;
}
