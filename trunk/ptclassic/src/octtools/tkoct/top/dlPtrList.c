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
/**
    dllPtrList

    Doubly-linked lists of pointers.  This is derived from dllList,
    and special cases it to when the object being stored is a pointer.
**/

#include "topFixup.h"
#include "topStd.h"

#include "topMem.h"
#include "topDlList.h"

typedef struct _DLPtrDestructorInfo DLPtrDestructorInfo;
struct _DLPtrDestructorInfo {
    DLElemDestructor	*elemDestructor;
    TOPPtr		userData;
};


static void
_dlPtrListElemDestructor(pList, pElem, userData)
    DLList		*pList;
    TOPPtr		pElem;
    TOPPtr		userData;
{
    DLPtrDestructorInfo	*pInfo = (DLPtrDestructorInfo*) userData;
    TOPPtr		*ppOb = (TOPPtr*) pElem;

    if ( pInfo == NULL || pInfo->elemDestructor == NULL )
	return;
    (*(pInfo->elemDestructor))(pList, *ppOb, pInfo->userData);
}

void
dlPtrListClear(pList, elemDestructor, userData)
    DLList		*pList;
    DLElemDestructor	*elemDestructor;
    TOPPtr		userData;
{
    DLPtrDestructorInfo	info;

    info.elemDestructor = elemDestructor;
    info.userData = userData;
    dlListClear( pList, 
      elemDestructor ? _dlPtrListElemDestructor : NULL, (TOPPtr)&info);
}

TOPStatus
dlPtrListConstructor(pList, name)
    DLList		*pList;
    char		*name;
{
    return dlListConstructor( pList, name, sizeof(TOPPtr));
}

void
dlPtrListDestructor(pList, elemDestructor, userData)
    DLList		*pList;
    DLElemDestructor	*elemDestructor;
    TOPPtr		userData;
{
    if (elemDestructor)
	dlPtrListClear(pList, elemDestructor, userData);
    dlListDestructor( pList, NULL, NULL);
}

DLList*
dlPtrListCreate(name)
    char		*name;
{
    return dlListCreate( name, sizeof(TOPPtr));
}

void
dlPtrListDestroy(pList, elemDestructor, userData)
    DLList		*pList;
    DLElemDestructor	*elemDestructor;
    TOPPtr		userData;
{
    if (elemDestructor)
	dlPtrListClear(pList, elemDestructor, userData);
    dlListDestroy( pList, NULL, NULL);
}

/**
	Search for the pointer {pCur} in the list, and return the
	reference to it.  Mainly for internal use.
**/
TOPPtr*
dlPtrListGetRef(pList, pCur)
    DLList		*pList;
    TOPPtr		pCur;
{
    DLLIST_LOOP_FORW_BEGIN(pList, TOPPtr*, ppOb) {
	if ( *ppOb == pCur )
	    return ppOb;
    } DLLIST_LOOP_FORW_END();
    return NULL;
}

/**
	Search for the pointer {pCur} in the list, and return
	TRUE if found somewhere in the list.
**/
TOPLogical
dlPtrListContainsB(pList, pCur)
    DLList		*pList;
    TOPPtr		pCur;
{
    return dlPtrListGetRef(pList, pCur) ? TOP_TRUE : TOP_FALSE;
}

/**
	Search for an element in the list, and if found, return
	the reference to it.  Mainly for internal use.
	The element is found when the {findCB} function returns 0
	between comparison of an element and {key}.
**/
TOPPtr*
dlPtrListFindRef(pList, findCB, key)
    DLList		*pList;
    DLFindFunc		*findCB;
    TOPPtr		key;
{
    DLLIST_LOOP_FORW_BEGIN(pList, TOPPtr*, ppOb) {
	if ( (*(findCB))( *ppOb, key) == 0 )
	    return ppOb;
    } DLLIST_LOOP_FORW_END();
    return NULL;
}

/**
	Search for an element in the list, and if found, return
	the pointer.
**/
TOPPtr
dlPtrListFind(pList, findCB, key)
    DLList		*pList;
    DLFindFunc		*findCB;
    TOPPtr		key;
{
    TOPPtr		*ppOb;

    ppOb = dlPtrListFindRef(pList, findCB, key);
    return ppOb ? *ppOb : NULL;
}
