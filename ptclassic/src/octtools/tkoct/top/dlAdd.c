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

#define DL_ALREADY_IN_LIST(pList) \
    topErrMsg("DLList: Element already in list (heap %s)", \
	  memHeapGetName(pList->heap))

#define DL_CUR_NOT_IN_LIST(pList) \
    topErrMsg("DLList: Current element not in list (heap %s)", \
	  memHeapGetName(pList->heap))

#define DL_ELEM_NOT_IN_LIST(pList) \
    topErrMsg("DLList: Element not in list (heap %s)", \
	  memHeapGetName(pList->heap))

TOPStatus
dlListElemRemove(pList, pCur)
    DLList		*pList;
    TOPPtr		pCur;
{
    DLElem		*pCurElem = DL_DATAPTR_TO_ELEM(pCur, -sizeof(DLElem));

    if (pCurElem->next == NULL) {
	DL_ELEM_NOT_IN_LIST(pList);
	return -1;
    }
    DL_REMOVE(pCurElem);
    DL_CLEAR(pCurElem);
    return 0;
}

TOPPtr
dlListRemoveAtHead(pList)
    DLList		*pList;
{
    DLElem		*pElem = pList->head.next;

    if (pElem == &pList->head)
	return NULL;
    DL_REMOVE(pElem);
    DL_CLEAR(pElem);
    return DL_ELEM_TO_DATAPTR(pElem, -sizeof(DLElem));
}

TOPPtr
dlListGetAtHead(pList)
    DLList		*pList;
{
    DLElem		*pElem = pList->head.next;

    if (pElem == &pList->head)
	return NULL;
    return DL_ELEM_TO_DATAPTR(pElem, -sizeof(DLElem));
}

TOPStatus
dlListInsertAtHead(pList, pNew)
    DLList		*pList;
    TOPPtr		pNew;
{
    DLElem		*pNewElem = DL_DATAPTR_TO_ELEM(pNew, -sizeof(DLElem));

     if (pNewElem->next != NULL) {
	DL_ALREADY_IN_LIST(pList);
	return -1;
    }
    DL_INSERT_AFTER(&pList->head, pNewElem);
    return 0;
}

TOPPtr
dlListCreateAtHead(pList)
    DLList		*pList;
{
    DLElem		*pNewElem;

    if ( (pNewElem = (DLElem*) memHeapObAlloc(pList->heap)) == NULL)
	return NULL;
    DL_INSERT_AFTER(&pList->head, pNewElem);
    return DL_ELEM_TO_DATAPTR(pNewElem, -sizeof(DLElem));
}

TOPStatus
dlListInsertAtTail(pList, pNew)
    DLList		*pList;
    TOPPtr		pNew;
{
    DLElem		*pNewElem = DL_DATAPTR_TO_ELEM(pNew, -sizeof(DLElem));

    if (pNewElem->next != NULL) {
	DL_ALREADY_IN_LIST(pList);
	return -1;
    }
    DL_INSERT_BEFORE(&pList->head, pNewElem);
    return 0;
}

TOPPtr
dlListCreateAtTail(pList)
    DLList		*pList;
{
    DLElem		*pNewElem;

    if ( (pNewElem = (DLElem*) memHeapObAlloc(pList->heap)) == NULL )
	return NULL;
    DL_INSERT_BEFORE(&pList->head, pNewElem);
    return DL_ELEM_TO_DATAPTR(pNewElem, -sizeof(DLElem));
}

TOPPtr
dlListGetAtTail(pList)
    DLList		*pList;
{
    DLElem		*pElem = pList->head.prev;

    if (pElem == &pList->head)
	return NULL;
    return DL_ELEM_TO_DATAPTR(pElem, -sizeof(DLElem));
}

TOPStatus
dlListInsertAfter(pList, pCur, pNew)
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
dlListCreateAfter(pList, pCur)
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

TOPPtr
dlListGetAfter(pList, pCur)
    DLList		*pList;
    TOPPtr		pCur;
{
    DLElem		*pCurElem = DL_DATAPTR_TO_ELEM(pCur, -sizeof(DLElem));

    if (pCurElem->next == NULL) {
	DL_CUR_NOT_IN_LIST(pList);
	return NULL;
    }

    if (pCurElem->next == &pList->head)
	return NULL;
    return DL_ELEM_TO_DATAPTR(pCurElem->next, -sizeof(DLElem));
}

TOPStatus
dlListInsertBefore(pList, pCur, pNew)
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
dlListCreateBefore(pList, pCur)
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


TOPPtr
dlListGetBefore(pList, pCur)
    DLList		*pList;
    TOPPtr		pCur;
{
    DLElem		*pCurElem = DL_DATAPTR_TO_ELEM(pCur, -sizeof(DLElem));

    if (pCurElem->next == NULL) {
	DL_CUR_NOT_IN_LIST(pList);
	return NULL;
    }

    if (pCurElem->prev == &pList->head)
	return NULL;
    return DL_ELEM_TO_DATAPTR(pCurElem->prev, -sizeof(DLElem));
}
