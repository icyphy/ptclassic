#if !defined(_TOPDLLIST_H_)
#define _TOPDLLIST_H_ 1
/**
    RCS: $Header$
    This header file requires "dlMacros.h".

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
**/

#include "dlMacros.h"

typedef struct _DLList DLList, *DLListHandle;
typedef struct _DLList DLPtrList, *DLPtrListHandle;

typedef void (DLElemDestructor)(DLList *pList, TOPPtr pElem, TOPPtr userData);

struct _DLList {
    DLElem		head;
    MEMHeap		*heap;
};

#define DLLIST_LOOP_FORW_BEGIN(pList, itemType, itemName) \
    DL_OFS_LOOP_FORW_BEGIN(&(pList)->head, itemType, itemName, -sizeof(DLElem))

#define DLLIST_LOOP_FORW_END() \
    DL_OFS_LOOP_FORW_END()

#define DLPTRLIST_LOOP_FORW_BEGIN(pList, itemType, itemName) \
    DLLIST_LOOP_FORW_BEGIN((pList), TOPPtr*, __dlptrlist_ppob__) { \
	itemType itemName = (itemType)*__dlptrlist_ppob__; {

#define DLPTRLIST_LOOP_FORW_END() \
    } } DLLIST_LOOP_FORW_END()

typedef int (DLFindFunc)( TOPPtr object, TOPPtr key);

/* dlList.c */
extern TOPStatus	dlListConstructor ARGS((DLList *pList, 
			  char *name, int objectSize));
extern void		dlListDestructor ARGS((DLList *pList,
			  DLElemDestructor *func, TOPPtr userData));

extern DLList*		dlListCreate ARGS((char *name, int objectSize));
extern void		dlListDestroy ARGS((DLList *pList, 
			  DLElemDestructor *func, TOPPtr userData));
extern void		dlListClear ARGS((DLList *pList,
			  DLElemDestructor *func, TOPPtr userData));

extern TOPPtr		dlListElemCreate ARGS((DLList *pList));
extern TOPStatus	dlListElemDestroy ARGS((DLList *pList, TOPPtr pCur));

extern TOPLogical	dlListEmptyB ARGS((DLList *pList));
extern int		dlListSize ARGS((DLList *pList));
extern TOPLogical	dlListElemValidB ARGS((DLList *pList, TOPPtr pCur));
extern TOPPtr		dlListFind ARGS((DLList *pList, DLFindFunc findCB,
			  TOPPtr key));
extern TOPPtr		dlListGetNth ARGS((DLList *pList, int n));



/* dlAdd.c */
extern TOPStatus	dlListElemRemove ARGS((DLList *pList, TOPPtr pCur));

extern TOPPtr		dlListRemoveAtHead ARGS((DLList *pList));
extern TOPPtr		dlListGetAtHead ARGS((DLList *pList));
extern TOPPtr		dlListCreateAtHead ARGS((DLList *pList));
extern TOPStatus	dlListInsertAtHead ARGS((DLList *pList, TOPPtr pNew));

extern TOPPtr		dlListCreateAtTail ARGS((DLList *pList));
extern TOPStatus	dlListInsertAtTail ARGS((DLList *pList, TOPPtr pNew));
extern TOPPtr		dlListGetAtTail ARGS((DLList *pList));

extern TOPPtr		dlListCreateAfter(/* pList, pCur */);
extern TOPStatus	dlListInsertAfter(/* pList, pCur, pNew */);
extern TOPPtr		dlListGetAfter ARGS((DLList *pList, TOPPtr pCur));

extern TOPPtr		dlListCreateBefore(/* pList, pCur */);
extern TOPStatus	dlListInsertBefore(/* pList, pCur, pNew */);
extern TOPPtr		dlListGetBefore ARGS((DLList *pList, TOPPtr pCur));

/* dlPtrList.c */

extern TOPStatus	dlPtrListConstructor ARGS((DLList *pList, char *name));
extern void		dlPtrListDestructor ARGS((DLList *pList,
			  DLElemDestructor *func, TOPPtr userData));

extern DLList*		dlPtrListCreate ARGS((char *name));
extern void		dlPtrListDestroy ARGS((DLList *pList, 
			  DLElemDestructor *func, TOPPtr userData));
extern void		dlListClear ARGS((DLList *pList,
			  DLElemDestructor *func, TOPPtr userData));

#define			dlPtrListEmptyB	dlListEmptyB
#define			dlPtrListSize	dlListSize

extern TOPPtr*		dlPtrListGetRef ARGS((DLList *pList, TOPPtr pCur));
extern TOPLogical	dlPtrListContainsB ARGS((DLList *pList, TOPPtr pCur));
extern TOPPtr*		dlPtrListFindRef ARGS((DLList *pList, DLFindFunc findCB,
			  TOPPtr key));
extern TOPPtr		dlPtrListFind ARGS((DLList *pList, DLFindFunc findCB,
			  TOPPtr key));


/* dlPtrAdd.c */
extern TOPStatus	dlPtrListElemRemove ARGS((DLList *pList, TOPPtr pCur));

extern TOPPtr		dlPtrListRemoveAtHead ARGS((DLList *pList));
extern TOPPtr		dlPtrListGetAtHead ARGS((DLList *pList));
extern TOPStatus	dlPtrListInsertAtHead ARGS((DLList *pList, TOPPtr pNew));

extern TOPStatus	dlPtrListInsertAtTail ARGS((DLList *pList, TOPPtr pNew));

#endif /* _TOPDLLIST_H_ */
