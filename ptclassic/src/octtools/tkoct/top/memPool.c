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

#include "topFixup.h"

#include "topStd.h"
#include "dlMacros.h"
#include "topMem.h"
#include "memCore.h"
#include "memStats.h"


/*****************************************************************************
 *
 *		MEMPool/MEMPoolSeg functions
 *
 *****************************************************************************/

#define MEM_POOLSEG_MINSIZE	(1<<8)		/* 256 */
#define MEM_POOLSEG_MAXSIZE	(1<<14)		/* 16 KB */
#define MEM_MALLOC_HDR_SIZE	12


/* this is a custom version of memSuperHeapObAlloc for storing the
   memPOOLSEG headers.  We could use the normal version and play
   and few games to break the recursion, but instead we use
   our own version to make things more independent. 
   This allocates a data block used to store memPOOLSEG structures,
   and is part of the _MemState.rootPoolsegs psuedo-heap.
   The weirdness is that the first element of the data block allocated
   here is actually the memPOOLSEG header for the block itself.
*/
static
MEMPoolSeg*
_memPoolSegHeaderAlloc()
{
    MEMSuperHeap	*pSuper = &_MemCore.rootPoolSegs;
    DLElem		*pRootSegChain = &_MemCore.headPoolSegSegs;
    MEMPool		*pPool = &pSuper->pool;
    MEMPoolSeg		*pSeg;
    MEMPoolFree		*pFree;
    unsigned		as;
    TOPPtr		next;

    /*
     * The idea here is very similar to memSuperHeapObAlloc
     */
    if (pSuper->freelist != NULL) {
	MEM_POOL_FREE_POP(&pSuper->freelist, &pSeg);
    } else {
	as = pPool->poolsegSize;
	if ((pSeg = (MEMPoolSeg*) malloc(as)) == NULL) {
	    memMallocFail( as, "MEMPoolSeg header segment");
	    return NULL;
	}
	/*
	 * This is weird: we take the first slot in the pool seg,
	 * and make it the control header for the pool seg (itself).
	 */
	pSeg->start = ((TOPPtr)pSeg);
	pSeg->cur   = ((TOPPtr)pSeg) + sizeof(*pSeg);
	pSeg->end   = ((TOPPtr)pSeg) + as;
	DL_INSERT_BEFORE(pRootSegChain, &pSeg->chain);
	/* now cram the rest in */
	pFree = pSuper->freelist;
	/*SUPPRESS 26*/ /*SUPPRESS 29*/ /*store & cmp bad ptr*/
	for ( ; (next=(pSeg->cur+sizeof(MEMPoolSeg))) < pSeg->end;
	  pSeg->cur = next) {
	    MEM_POOL_FREE_PUSH_F(&pFree, &pSeg->cur, MEM_POOL_MAGIC_WASTED);
	    ++(pSuper->totCrams);
	}
	/* ... and pull off the one we actually want to return */
	--(pSuper->totCrams);
	pSeg = (MEMPoolSeg*) pFree;
	pSuper->freelist = pFree->next;
    }
    pSeg->start = pSeg->cur = pSeg->end = NULL;
    ++(pSuper->totAllocs);
    return pSeg;
}

MEMPoolSeg*
_memPoolSegAlloc(pPool, allocSize)
    MEMPool		*pPool;
    unsigned		allocSize;
{
    MEMPoolSeg		*pSeg;
    int			ps = pPool->poolsegSize;
    unsigned		as = allocSize;

    if ((pSeg = _memPoolSegHeaderAlloc()) == NULL) {
	return NULL;
    }
    if (as < ps) {
	as = ps;
        ps = (ps+MEM_MALLOC_HDR_SIZE) * 2;
        if (ps > MEM_POOLSEG_MAXSIZE)
	    ps = MEM_POOLSEG_MAXSIZE;
        pPool->poolsegSize = ps - MEM_MALLOC_HDR_SIZE;
    }
    if ((pSeg->start = (TOPPtr) malloc(as)) == NULL) {
	/* XXX: drop the seg header */
	memMallocFail( as, "MEMPool segment");
	return NULL;
    }
    DL_INSERT_BEFORE(&pPool->chainHead, &pSeg->chain);
    pSeg->cur = pSeg->start;
    pSeg->end = pSeg->start + as;
    return pSeg;
}

static void
_memPoolSegFree(pPool, pSeg)
    MEMPool		*pPool;
    MEMPoolSeg		*pSeg;
{
    MEMSuperHeap	*pSuper = &_MemCore.rootPoolSegs;

    DL_REMOVE(&pSeg->chain);
    if (pSeg->start != NULL) {
	free(pSeg->start);
    }
    pSeg->start = pSeg->cur = pSeg->end = NULL;
    MEM_POOL_FREE_PUSH_F(&pSuper->freelist, &pSeg, MEM_POOL_MAGIC_FREE);
    ++(pSuper->totFrees);
}
    
void
memPoolConstructor(pPool, initialPoolSize)
    MEMPool		*pPool;
    unsigned		initialPoolSize;
{
    int			ps;
    int			m;

    memset((char*)pPool, 0, sizeof(*pPool));
    ps = initialPoolSize;
    if (ps < MEM_POOLSEG_MINSIZE) {
	ps = MEM_POOLSEG_MINSIZE;
    } else if (ps > MEM_POOLSEG_MAXSIZE) {
	ps = MEM_POOLSEG_MAXSIZE;
    } else {
	for (m=1; (ps & ~(m-1)) != 0; m <<= 1)
	    ;
	ps = m;
    }
    pPool->poolsegSize = ps - MEM_MALLOC_HDR_SIZE;
    DL_INIT_HEAD(&pPool->chainHead);
}

void
memPoolDestructor(pPool)
    MEMPool		*pPool;
{
    DLElem		*pHead = &pPool->chainHead;
    DLElem		*pCur;
    MEMPoolSeg		*pSeg;

    for (pCur=pHead->next; pCur != pHead; ) {
	pSeg = DL_SELF(MEMPoolSeg*, pCur);
	pCur = pCur->next;
   	_memPoolSegFree(pPool, pSeg);
    }
    DL_CLEAR(pHead);
}

TOPPtr
memPoolObTryAlloc(pPool, allocSize)
    MEMPool		*pPool;
    unsigned		allocSize;
{
    MEMPoolSeg		*pSeg;
    TOPPtr		pOb;
    TOPPtr		next;

    pSeg = DL_PREV(MEMPoolSeg*, &pPool->chainHead);
    if (pSeg == NULL || pSeg->cur == NULL
      || (next=(pSeg->cur+allocSize)) >= pSeg->end) {
	return NULL;
    }
    pOb = pSeg->cur;
    pSeg->cur = next;
    return pOb;
}

TOPPtr
memPoolObAlloc(pPool, allocSize)
    MEMPool		*pPool;
    unsigned		allocSize;
{
    MEMPoolSeg		*pSeg;
    TOPPtr		pOb;
    TOPPtr		next;

    pSeg = DL_PREV(MEMPoolSeg*, &pPool->chainHead);
    if (pSeg == NULL || pSeg->cur == NULL
      || (next=(pSeg->cur+allocSize)) >= pSeg->end) {
	if ((pSeg = _memPoolSegAlloc(pPool, allocSize)) == NULL) {
	    return NULL;
	}
        next = pSeg->cur + allocSize;
    }
    pOb = pSeg->cur;
    pSeg->cur = next;
    return pOb;
}

TOPLogical
memPoolObValidB(pPool, pOb, pOfs)
    MEMPool		*pPool;
    TOPPtr		pOb;
    int			*pOfs;
{
    MEMPoolSeg		*pSeg;
    DLElem		*pHead = & pPool->chainHead;
    DLElem		*pSegElem;

    for (pSegElem=pHead->next; pSegElem != pHead; pSegElem=pSegElem->next) {
	pSeg = DL_SELF(MEMPoolSeg*, pSegElem);
	if (pOb >= MEM_POOLSEG_START(pSeg) && pOb < pSeg->cur) {
	    if ( pOfs ) {
		*pOfs = pOb - pSeg->cur;
	    }
	    return TOP_TRUE;
	}
    }
    return TOP_FALSE;
}

void
memPoolStatsCalc(pPool, pStats)
    MEMPool		*pPool;
    MEMPoolStats	*pStats;
{
    MEMPoolSeg		*pSeg;
    DLElem		*pHead = &pPool->chainHead;
    DLElem		*pSegElem;

    memset((char*)pStats, 0, sizeof(*pStats));
    for (pSegElem=pHead->next; pSegElem != pHead; pSegElem=pSegElem->next) {
	pSeg = DL_SELF(MEMPoolSeg*, pSegElem);
	++(pStats->numSegs);
	pStats->mem.live.bytes += pSeg->cur - pSeg->start;
	pStats->mem.wasted.bytes += pSeg->end - pSeg->cur;
	pStats->mem.overhead.bytes += sizeof(MEMPoolSeg) + MEM_MALLOC_HDR_SIZE;
    }
    pStats->mem.overhead.bytes += sizeof(MEMPool);
    memStdByteStatsCalc(&pStats->mem, 0);
}

void
memPoolStatsPrint(pStats)
    MEMPoolStats	*pStats;
{
    char		buf[100];

    sprintf(buf, "Pool Memory Usage (%d segs)\n", pStats->numSegs);
    memStdByteStatsPrint(buf, &pStats->mem);
}
