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
#include "topVec.h"


int
topVecGetSize( TOPVector *pVec) {
    return pVec->NumAlloc;
}

void
topVecSetSize( TOPVector *pVec, unsigned n) {
    int			old_alloc;

    if ( n == 0 ) {
	MEM_FREE(pVec->Data);
	pVec->Data = NULL;
	pVec->NumAlloc = 0;
	return;
    }

    old_alloc = pVec->NumAlloc;
    pVec->NumAlloc = n;
    if ( pVec->Data == NULL ) {
	pVec->Data = memCalloc(pVec->NumAlloc, pVec->ObSize);
    } else {
	pVec->Data = memRealloc(pVec->Data, pVec->NumAlloc * pVec->ObSize);
    }
    if ( pVec->NumAlloc > old_alloc ) {
	memset( ((char*)(pVec->Data)) + old_alloc*pVec->ObSize, 0, 
	  (pVec->NumAlloc - old_alloc)*pVec->ObSize);
    }
}

TOPLogical
topVecInsureSize( TOPVector *pVec, unsigned n) {
    if ( ((int)n) < 0 || ((int)pVec->Num) < 0 ) {
	topAbortMsg("topVecInsureSize: negative size (%d %d %d)",
	  n, pVec->Num, pVec->NumAlloc);
    }
    if ( n <= pVec->NumAlloc )
	return TOP_FALSE;
    if ( n < 16 )
	n = 16;
    if ( n < 2 * pVec->NumAlloc )
	n = 2 * pVec->NumAlloc;
    topVecSetSize( pVec, n);
    return TOP_TRUE;
}

void
topVecConstructor( TOPVector *pVec, unsigned obSize) {
    pVec->ObSize = obSize;
    pVec->Num = pVec->NumAlloc = 0;
    pVec->Data = NULL;
}

void
topVecDestructor( TOPVector *pVec) {
    static char		func[] = "topVecDestructor";

    if ( pVec->Num == TOPVEC_MAGIC_BAD ) {
	topProgMsg( "%s: Duplicate destruction",func);
	return;
    }
    topVecSetSize( pVec, 0);
    pVec->Num = TOPVEC_MAGIC_BAD;
}

int
topVecGetNum( TOPVector *pVec) {
    return pVec->Num;
}

void
topVecSetNum( TOPVector *pVec, unsigned n) {
    topVecInsureSize( pVec, n);
    pVec->Num = n;
}

void
topVecAddNum( TOPVector *pVec, unsigned n) {
    topVecInsureSize( pVec, pVec->Num+n);
    pVec->Num += n;
}

TOPLogical
topVecInsureNum( TOPVector *pVec, unsigned n) {
    if ( n <= pVec->Num )
	return TOP_FALSE;
    topVecSetNum( pVec, n);
    return TOP_TRUE;
}

void
topVecClear( TOPVector *pVec) {
    pVec->Num = 0;
    topVecSetSize( pVec, 0);
}

TOPLogical
topVecEmptyB( TOPVector *pVec) {
    return pVec->Num == 0;
}

void
topVecCopy( TOPVector *pDst, TOPVector *pSrc) {
    topVecSetNum( pDst, pSrc->Num);
    if ( pSrc->Num > 0 ) {
        memcpy( pDst->Data, pSrc->Data, pSrc->Num * pSrc->ObSize);
    }
}

int
topVecAppendSizeIdx( TOPVector *pVec, unsigned n) {
    topVecInsureSize( pVec, pVec->Num + n);
    return pVec->Num;
}

int
topVecAppendIdx( TOPVector *pVec) {
    topVecInsureSize( pVec, ++(pVec->Num));
    return (pVec->Num) - 1;
}

TOPPtr
topVecAppend( TOPVector *pVec) {
    topVecInsureSize( pVec, ++(pVec->Num));
    return pVec->Data + (pVec->Num-1) * pVec->ObSize;
}


/**
    Insert {num} elements at index {idx} to {pVec}.  The inserted
    elements are not initialized in any way.
**/
int
topVecInsertIdx( TOPVector *pVec, unsigned idx, unsigned num) {
    int		k, num_shift;
    char	*src, *tgt;

    pVec->Num += num;
    topVecInsureSize( pVec, pVec->Num);
    if ( (num_shift = pVec->Num - idx) > 0 ) {
	src = pVec->Data + idx * pVec->ObSize;
	tgt = src + pVec->ObSize * num;
	for (k=pVec->ObSize * num_shift - 1; k >= 0; k--) {
	    tgt[k] = src[k];
	}
    }
    return idx;
}

/**
   Remove {num} elements, starting at index {idx} from {pVec}.
**/
int
topVecRemoveIdx( TOPVector *pVec, unsigned idx, unsigned num) {
    int		k, num_shift;
    char	*src, *tgt;

    if ( ((signed)idx) < 0 || idx >= pVec->Num )
	return 0;
    if ( (num_shift = pVec->Num - idx - num) > 0 ) {
	tgt = pVec->Data + idx * pVec->ObSize;
	src = tgt + pVec->ObSize * num;
	for (k=pVec->ObSize * num_shift - 1; k >= 0; k--) {
	    *tgt++ = *src++;
	}
        pVec->Num -= num;
    } else {
	pVec->Num = idx;
    }
    return idx;
}


/**
    Shift the entire vector by k:
       (pos k ==> grows, introducing null elements at head)
       (neg k ==> shrinks, dropping elements at head)
   The caller must initialize the null elements at the head for pos k.
   Returns number of elements in vector.
**/
int
topVecShift( TOPVector *pVec, int k) {
    if ( k < 0 ) {
	topVecRemoveIdx( pVec, 0, (unsigned)-k);
    } else if ( k > 0 ) {
	topVecInsertIdx( pVec, 0, (unsigned)k);
    }
    return pVec->Num;
}

/******************************************************************************

	TOPPtrVector

******************************************************************************/

void
topPtrVecConstructor( TOPPtrVector *pVec) {
    topVecConstructor( pVec, sizeof(TOPPtr));
}

void
topPtrVecDestructor( TOPPtrVector *pVec) {
    topVecDestructor( pVec);
}

/*
   Return FALSE if it already exists in the vector, else TRUE
*/
TOPLogical
topPtrVecAppendUnique( TOPPtrVector *pVec, TOPPtr pOb) {
    TOPPTRVEC_LOOP_FORW_BEGIN( pVec, TOPPtr, vecOb) {
	if ( vecOb == pOb )
	    return TOP_FALSE;
    } TOPPTRVEC_LOOP_FORW_END();
    TOPVEC_APPEND( pVec, TOPPtr) = pOb;
    return TOP_TRUE;
}
