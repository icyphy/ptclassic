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

#include "topMem.h"
#include "topDlList.h"

/**
	Look for pointer {pCur} in list, and if found, remove it
	and return 0.  If not found, return 1.  If error, return -1.
**/
TOPStatus
dlPtrListElemRemove(pList, pCur)
    DLList		*pList;
    TOPPtr		pCur;
{
    TOPPtr		*ppOb;

    ppOb = dlPtrListGetRef(pList, pCur);
    if ( ppOb == NULL )
	return 1;
    if ( dlListElemDestroy(pList, (TOPPtr)ppOb) < 0 )
	return -1;
    return 0;
}

TOPPtr
dlPtrListRemoveAtHead(pList)
    DLList		*pList;
{
    TOPPtr		*ppOb;

    ppOb = (TOPPtr*) dlListRemoveAtHead(pList);
    return ppOb ? *ppOb : NULL;
}

TOPPtr
dlPtrListGetAtHead(pList)
    DLList		*pList;
{
    TOPPtr		*ppOb;

    ppOb = (TOPPtr*) dlListGetAtHead(pList);
    return ppOb ? *ppOb : NULL;
}

TOPStatus
dlPtrListInsertAtHead(pList, pNew)
    DLList		*pList;
    TOPPtr		pNew;
{
    TOPPtr		*ppOb;

    ppOb = (TOPPtr*) dlListCreateAtHead(pList);
    if ( ppOb == NULL )
	return -1;
     *ppOb = pNew;
    return 0;
}

TOPStatus
dlPtrListInsertAtTail(pList, pNew)
    DLList		*pList;
    TOPPtr		pNew;
{
    TOPPtr		*ppOb;

    ppOb = (TOPPtr*) dlListCreateAtTail(pList);
    if ( ppOb == NULL )
	return -1;
     *ppOb = pNew;
    return 0;
}

#ifdef notdef
TOPStatus
dlPtrListInsertAfter(pList, pCur, pNew)
    DLList		*pList;
    TOPPtr		pCur;
    TOPPtr		pNew;
{
    DLElem		*pCurElem = DL_DATAPTR_TO_ELEM(pCur, -sizeof(DLElem));
    DLElem		*pNewElem = DL_DATAPTR_TO_ELEM(pNew, -sizeof(DLElem));

    if (pCurElem->next == NULL) {
	DL_CUR_NOT_IN_LIST(pList);
	return -1;
    }
    if (pNewElem->next != NULL) {
	DL_ALREADY_IN_LIST(pList);
	return -1;
    }
    DL_INSERT_AFTER(pCurElem, pNewElem);
    return 0;
}

TOPPtr
dlPtrListCreateAfter(pList, pCur)
    DLList		*pList;
    TOPPtr		pCur;
{
    DLElem		*pCurElem = DL_DATAPTR_TO_ELEM(pCur, -sizeof(DLElem));
    DLElem		*pNewElem;

    if (pCurElem->next == NULL) {
	DL_CUR_NOT_IN_LIST(pList);
	return NULL;
    }
    if ( (pNewElem = (DLElem*) memHeapObAlloc(pList->heap)) == NULL )
	return NULL;
    DL_INSERT_AFTER(pCurElem, pNewElem);
    return DL_ELEM_TO_DATAPTR(pNewElem, -sizeof(DLElem));
}


TOPStatus
dlPtrListInsertBefore(pList, pCur, pNew)
    DLList		*pList;
    TOPPtr		pCur;
    TOPPtr		pNew;
{
    DLElem		*pCurElem = DL_DATAPTR_TO_ELEM(pCur, -sizeof(DLElem));
    DLElem		*pNewElem = DL_DATAPTR_TO_ELEM(pNew, -sizeof(DLElem));

    if (pCurElem->next == NULL) {
	DL_CUR_NOT_IN_LIST(pList);
	return -1;
    }
    if (pNewElem->next != NULL) {
	DL_ALREADY_IN_LIST(pList);
	return -1;
    }
    DL_INSERT_BEFORE(pCurElem, pNewElem);
    return 0;
}

TOPPtr
dlPtrListCreateBefore(pList, pCur)
    DLList		*pList;
    TOPPtr		pCur;
{
    DLElem		*pCurElem = DL_DATAPTR_TO_ELEM(pCur, -sizeof(DLElem));
    DLElem		*pNewElem;

    if (pCurElem->next == NULL) {
	DL_CUR_NOT_IN_LIST(pList);
	return NULL;
    }
    if ( (pNewElem = (DLElem*) memHeapObAlloc(pList->heap)) == NULL)
	return NULL;
    DL_INSERT_BEFORE(pCurElem, pNewElem);
    return DL_ELEM_TO_DATAPTR(pNewElem, -sizeof(DLElem));
}

#endif
