#if !defined(_TOPVEC_H_)
#define _TOPVEC_H_ 1
/* 
    RCS: $Header$
    Author: Kennard White

    Copyright (c) 1990-93 The Regents of the University of California.
    All rights reserved.  See the file "./copyright.h" for full copyright
    notice, limitation of liability, and disclaimer of warranty provisions.
#include "copyright.h"				COPYRIGHTENDKEY
*/

#if !defined(_TOPSTD_H_) && __TOP_AUTOINCLUDE__
#include "topStd.h"
#endif


/**
**/

typedef struct _TOPVector TOPVector;
typedef TOPVector TOPPtrVector;

struct _TOPVector {
    unsigned	ObSize;
    unsigned	Num;
    unsigned	NumAlloc;
    TOPPtr	Data;

};

#define TOPVEC_MAGIC_BAD ((unsigned)-3)

/******************************************************************************
 *
 *	Macros for accessing data in the array.  These generally fall into
 *	two clasess:
 *	"fast" macros: these cast the array based on the type-argument,
 *		leading to compilier optimization of array indexing.
 *	"slow" macros: these do array indexing explicttly using {ObSize};
 *		these are slower, but you dont have to know what you are
 *		indexing inorder to do it.
 *
 *****************************************************************************/

#define TOPVEC_Num( pVec)	( (pVec)->Num )
#define TOPVEC_NUM	TOPVEC_Num
#define TOPVEC_GETNUM	TOPVEC_Num

#define TOPVEC_SGet( pVec, i, type) \
    ( *TOP_Cast(type*,(pVec)->Data+((i)*(pVec)->ObSize)) )

#define TOPVEC_Get( pVec, i, type) ( ((type*)((pVec)->Data))[(i)] )
#define TOPVEC_GET	TOPVEC_Get

#define TOPVEC_HEAD( pVec, type) \
    TOPVEC_GET( (pVec), 0, type)

#define TOPVEC_TAIL( pVec, type) \
    TOPVEC_GET( (pVec), ((pVec)->Num)-1, type)

/* sucessor to tail */
#define TOPVEC_TAILSUC( pVec, type) \
    TOPVEC_GET( (pVec), ((pVec)->Num), type)

#define TOPVEC_Array( pVec, type) ( (type*)((pVec)->Data) )
#define TOPVEC_GETARRAY	TOPVEC_Array


#define TOPVEC_SET( pVec, i, type) (	\
    topVecInsureNum((pVec), (i)+1),	\
    TOPVEC_GET( (pVec), (i), type)	\
)

#define TOPVEC_Append( pVec, type) \
    ( TOPVEC_GET( (pVec), topVecAppendIdx(pVec), type) )
#define TOPVEC_APPEND TOPVEC_Append

#define TOPVEC_APPEND_INSURE( pVec, n, type) \
    ( topVecInsureSize((pVec), (pVec)->Num+(n)), TOPVEC_TAILSUC((pVec), type) )

#define TOPVEC_INSERT( pVec, idx, type) \
    ( TOPVEC_GET( (pVec), topVecInsertIdx((pVec), (idx), 1), type) )

/*
 * TOPVector iteration macros
 */

/*
 * VEC: SUBIDX LOOP
 */
#define TOPVEC_SUBIDX_LOOP_FORW_BEGIN( pVec, sI, fI, idxName, type, ptrName) { \
    int		idxName;						\
    type*	ptrName;						\
    for (idxName=(sI); idxName < (fI); idxName++) {			\
	ptrName = & TOPVEC_GET(pVec, idxName, type);			\
        /* BEGIN APP CODE */

#define TOPVEC_SUBIDX_LOOP_FORW_END() } }

#define TOPVEC_SUBIDX_LOOP_BACK_BEGIN( pVec, sI, fI, idxName, type, ptrName) { \
    int		idxName;						\
    type*	ptrName;						\
    for (idxName=(fI)-1; idxName >= (sI); idxName--) {			\
	ptrName = & TOPVEC_GET(pVec, idxName, type);			\
        /* BEGIN APP CODE */

#define TOPVEC_SUBIDX_LOOP_BACK_END() } }

/*
 * VEC: IDX LOOP
 */
#define TOPVEC_IDX_LOOP_FORW_BEGIN( pVec, idxName, type, ptrName) \
    TOPVEC_SUBIDX_LOOP_FORW_BEGIN( (pVec), 0, (pVec)->Num, \
      idxName, type, ptrName) \
        /* BEGIN APP CODE */

#define TOPVEC_IDX_LOOP_FORW_END() \
    TOPVEC_SUBIDX_LOOP_FORW_END()


#define TOPVEC_IDX_LOOP_BACK_BEGIN( pVec, idxName, type, ptrName) \
    TOPVEC_SUBIDX_LOOP_BACK_BEGIN( (pVec), 0, (pVec)->Num, \
      idxName, type, ptrName) \
        /* BEGIN APP CODE */

#define TOPVEC_IDX_LOOP_BACK_END() \
    TOPVEC_SUBIDX_LOOP_BACK_END()

/*
 * VEC: LOOP
 */
#define TOPVEC_LOOP_FORW_BEGIN( pVec, type, obPtrName) \
    TOPVEC_IDX_LOOP_FORW_BEGIN( (pVec), __topvec_index_i_, type, obPtrName)

#define TOPVEC_LOOP_FORW_END() \
    TOPVEC_IDX_LOOP_FORW_END()

#define TOPVEC_LOOP_BACK_BEGIN( pVec, type, obPtrName) \
    TOPVEC_IDX_LOOP_BACK_BEGIN( (pVec), __topvec_index_i_, type, obPtrName)

#define TOPVEC_LOOP_BACK_END() \
    TOPVEC_IDX_LOOP_BACK_END()

/*
 * PTRVEC: SUBIDX LOOP
 */
#define TOPPTRVEC_SUBIDX_LOOP_FORW_BEGIN( pVec,sI,fI, idxName, type, ptrName){ \
    type	ptrName;						  \
    TOPVEC_SUBIDX_LOOP_FORW_BEGIN( (pVec), (sI), (fI), idxName, type, __topvec_ptrname_) \
	ptrName = *__topvec_ptrname_;					  \
        /* BEGIN APP CODE */

#define TOPPTRVEC_SUBIDX_LOOP_FORW_END() \
    TOPVEC_SUBIDX_LOOP_FORW_END() }

#define TOPPTRVEC_SUBIDX_LOOP_BACK_BEGIN( pVec,sI,fI, idxName, type, ptrName){ \
    type	ptrName;						  \
    TOPVEC_SUBIDX_LOOP_BACK_BEGIN( (pVec), (sI), (fI), idxName, type, __topvec_ptrname_) \
	ptrName = *__topvec_ptrname_;					  \
        /* BEGIN APP CODE */

#define TOPPTRVEC_SUBIDX_LOOP_BACK_END() \
    TOPVEC_SUBIDX_LOOP_BACK_END() }

/*
 * PTRVEC: IDX LOOP
 */
#define TOPPTRVEC_IDX_LOOP_FORW_BEGIN( pVec, idxName, type, ptrName) \
    TOPPTRVEC_SUBIDX_LOOP_FORW_BEGIN( (pVec), 0, (pVec)->Num, idxName, \
      type, ptrName)

#define TOPPTRVEC_IDX_LOOP_FORW_END() \
    TOPPTRVEC_SUBIDX_LOOP_FORW_END()

#define TOPPTRVEC_IDX_LOOP_BACK_BEGIN( pVec, idxName, type, ptrName) \
    TOPPTRVEC_SUBIDX_LOOP_BACK_BEGIN( (pVec), 0, (pVec)->Num, idxName, \
      type, ptrName)

#define TOPPTRVEC_IDX_LOOP_BACK_END() \
    TOPPTRVEC_SUBIDX_LOOP_BACK_END()

/*
 * PTRVEC: LOOP
 */
#define TOPPTRVEC_LOOP_FORW_BEGIN( pVec, type, ptrName) \
    TOPPTRVEC_IDX_LOOP_FORW_BEGIN( (pVec), __topvec_index_i_, type, ptrName)

#define TOPPTRVEC_LOOP_FORW_END() \
    TOPPTRVEC_IDX_LOOP_FORW_END()

#define TOPPTRVEC_LOOP_BACK_BEGIN( pVec, type, ptrName) \
    TOPPTRVEC_IDX_LOOP_BACK_BEGIN( (pVec), __topvec_index_i_, type, ptrName)

#define TOPPTRVEC_LOOP_BACK_END() \
    TOPPTRVEC_IDX_LOOP_BACK_END()


/******************************************************************************
 *
 *	The iteration macros below dont use sizeof(type) to control
 *	the iteration.  Instead, they use {pVec->ObSize}.  This will generally
 *	lead to less efficient code.
 *
 *****************************************************************************/
#define TOPVEC_SUBIDX_SLOOP_FORW_BEGIN( pVec, sI, fI, idxName, type, ptrName){ \
    int		idxName;						\
    type*	ptrName;						\
    int		__topvec_obsize__ = (pVec)->ObSize;			\
    for (idxName=(sI); idxName < (fI); idxName++) {			\
	ptrName = TOP_Cast(type*,(pVec)->Data+(idxName*__topvec_obsize__)); \
        /* BEGIN APP CODE */


#define TOPVEC_SUBIDX_SLOOP_FORW_END() } }

#define TOPVEC_IDX_SLOOP_FORW_BEGIN( pVec, idxName, type, ptrName) \
    TOPVEC_SUBIDX_SLOOP_FORW_BEGIN( (pVec), 0, (pVec)->Num, \
      idxName, type, ptrName) \
        /* BEGIN APP CODE */

#define TOPVEC_IDX_SLOOP_FORW_END() \
    TOPVEC_SUBIDX_SLOOP_FORW_END()

/******************************************************************************
 *
 *		 Externs
 *
 *****************************************************************************/

/* topVec.c */
extern int		topVecGetSize ARGS((TOPVector *pVec));
extern void		topVecSetSize ARGS((TOPVector *pVec, unsigned n));
extern TOPLogical	topVecInsureSize ARGS((TOPVector *pVec, unsigned n));

extern void		topVecConstructor ARGS((TOPVector *pVec, unsigned obSize));
extern void		topVecDestructor ARGS((TOPVector *pVec));

extern TOPLogical	topVecInsureNum ARGS((TOPVector *pVec, unsigned n));
extern int		topVecGetNum ARGS((TOPVector *pVec));
extern void		topVecSetNum ARGS((TOPVector *pVec, unsigned n));
extern void		topVecAddNum ARGS((TOPVector *pVec, unsigned n));

extern void		topVecClear ARGS((TOPVector *pVec));
extern TOPLogical	topVecEmptyB ARGS((TOPVector *pVec));
extern void		topVecCopy ARGS((TOPVector *pDst, TOPVector *pSrc));

extern int		topVecAppendIdx ARGS((TOPVector *pVec));
extern TOPPtr		topVecAppend ARGS((TOPVector *pVec));
extern int		topVecInsertIdx ARGS((TOPVector *pVec,unsigned idx,unsigned num));
extern int		topVecRemoveIdx ARGS((TOPVector *pVec,unsigned idx,unsigned num));
extern int		topVecShift ARGS((TOPVector *pVec, int k));




extern void		topPtrVecConstructor ARGS((TOPVector *pVec));
extern void		topPtrVecDestructor ARGS((TOPVector *pVec));
extern TOPLogical	topPtrVecAppendUnique ARGS((TOPPtrVector *pVec, TOPPtr pOb));

extern void		topVecSort ARGS((TOPVector *pVec, TOPCompareFunc compFunc));
extern int		_topVecSortInt ARGS((const void *a, const void *b));
#define TOPVEC_SORT_INT	_topVecSortInt
extern int		_topVecSortDbl ARGS((const void *a, const void *b));
#define TOPVEC_SORT_DBL	_topVecSortDbl


#endif /* _TOPVEC_H_ */
