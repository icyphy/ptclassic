#if !defined(_DLMACROS_H_)
#define _DLMACROS_H_ 1
/**
    RCS: $Header$
    This file is included from dlList.h.

    Author: Kennard White

    Copyright (c) 1990-93 The Regents of the University of California.
    All rights reserved.  See the file "./copyright.h" for full copyright
    notice, limitation of liability, and disclaimer of warranty provisions.
#include "copyright.h"				COPYRIGHTENDKEY
**/

typedef struct _DLElem DLElem;
struct _DLElem {
    DLElem		*next;
    DLElem		*prev;
};

#define DL_SELF(type, pElem)	( (type) (pElem) )
#define DL_PREV(type, pElem)	DL_SELF(type, (pElem)->prev)
#define DL_NEXT(type, pElem)	DL_SELF(type, (pElem)->next)

#define DL_CLEAR(pElem) (void)(		\
    ((pElem)->prev = NULL),		\
    ((pElem)->next = NULL),		\
    (pElem)				\
)

#define DL_INIT_HEAD(pNew) (void)(	\
    ((pNew)->prev = (pNew)),		\
    ((pNew)->next = (pNew)),		\
    (pNew)				\
)

#define DL_INSERT_AFTER(pOld, pNew) (void)(	\
    ((pNew)->prev = (pOld)),		\
    ((pNew)->next = (pOld)->next),	\
    ((pNew)->prev->next = (pNew)),	\
    ((pNew)->next->prev = (pNew)),	\
    (pNew)				\
)

#define DL_INSERT_BEFORE(pOld, pNew) (void)(	\
    ((pNew)->prev = (pOld)->prev),	\
    ((pNew)->next = (pOld)),		\
    ((pNew)->prev->next = (pNew)),	\
    ((pNew)->next->prev = (pNew)),	\
    (pNew)				\
)


#define DL_REMOVE(pOld) (void)(			\
    ((pOld)->next->prev = (pOld)->prev),	\
    ((pOld)->prev->next = (pOld)->next),	\
    (NULL)					\
)

/* the offset is the distance from the start of the user-visible data
   to the start of the start of the DLElim structure 
*/

#define DL_DATAPTR_TO_ELEM(x, offset) \
    ( (DLElem*) (((char*)(x)) + (offset)) )

#define DL_ELEM_TO_DATAPTR(x, offset) \
    ( (TOPPtr)   (((char*)(x)) - (offset)) )


#define DL_OFS_LOOP_FORW_BEGIN(pHead, itemType, itemName, offset) {	\
    DLElem		*__dl_loop_elem, *__dl_loop_elem_next;		\
    for (								\
      __dl_loop_elem=(pHead)->next;					\
      __dl_loop_elem != (pHead);					\
      __dl_loop_elem = __dl_loop_elem_next) {				\
	itemType itemName = (itemType)					\
	  DL_ELEM_TO_DATAPTR(__dl_loop_elem, offset);			\
        __dl_loop_elem_next = __dl_loop_elem->next;			\
	/* BEGIN APP CODE */
	

#define DL_OFS_LOOP_FORW_END() /* END APP CODE */ } }

#define DLMAC_LOOP_FORW_BEGIN(pHead, itemType, itemName) \
    DL_OFS_LOOP_FORW_BEGIN(pHead, itemType, itemName, 0)

#define DLMAC_LOOP_FORW_END() \
    DL_OFS_LOOP_FORW_END()

#endif /* _DLMACROS_H_ */
