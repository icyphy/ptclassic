#if !defined(_DLMACROS_H_)
#define _DLMACROS_H_ 1
/**
    RCS: $Header$
    This file is included from dlList.h.

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
