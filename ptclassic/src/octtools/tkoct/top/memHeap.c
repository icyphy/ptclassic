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

#include "dlMacros.h"
#include "topMem.h"
#include "memCore.h"
#include "memStats.h"


/*****************************************************************************
 *
 *		MEMSuperHeap functions
 *
 *****************************************************************************/

void
memSuperHeapConstructor(pSuper, name, objectSize)
    MEMSuperHeap	*pSuper;
    char		*name;
    unsigned		objectSize;
{
    memset((char*)pSuper, 0, sizeof(*pSuper));
    pSuper->name = name;
    pSuper->objectSize = objectSize < MEM_SUPERHEAP_MINSIZE 
      ? MEM_SUPERHEAP_MINSIZE : objectSize;
    memPoolConstructor(&pSuper->pool, 10 * objectSize);
    if (*name == '^' && strncmp(name, "^root", 5)==0) {
        ; /* DONT put root heaps into global list: screws up memory stats */
    } else {
        ++(_MemCore.numSuperHeaps);
    	DL_INSERT_BEFORE(&_MemCore.headSuperHeaps, &pSuper->chain);
    }
    DL_INIT_HEAD(&pSuper->headHeapChain);
}

void
memSuperHeapDestructor(pSuper)
    MEMSuperHeap	*pSuper;
{
    int			heapCnt = 0;

    memPoolDestructor(&pSuper->pool);
    DLMAC_LOOP_FORW_BEGIN(&pSuper->headHeapChain, MEMHeap*, pHeap) {
	pHeap->super = NULL;
	++heapCnt;
    } DLMAC_LOOP_FORW_END();
    DL_CLEAR(&pSuper->headHeapChain);
    if (heapCnt > 0) {
        topWarnMsg("SH ``%s'' destructed with %d active heaps attached",
	  pSuper->name, heapCnt);
    }
    if (pSuper->totAllocs != pSuper->totFrees) {
    	topWarnMsg("SH ``%s'' destructed when not empty (%d vs %d)",
	  pSuper->name, pSuper->totAllocs, pSuper->totFrees);
    }
    if (pSuper->chain.next != NULL) {
    	DL_REMOVE(&pSuper->chain);    /* remove from _MemCore.headSuperHeaps */
	DL_CLEAR(&pSuper->chain);
    	--(_MemCore.numSuperHeaps);
    }
}

MEMSuperHeap*
memSuperHeapCreate(name, objectSize)
    char		*name;
    unsigned		objectSize;
{
    MEMSuperHeap	*pSuper;

    MEM_ASSERT_INITED();
    if ((pSuper = (MEMSuperHeap*)memSuperHeapObAlloc(&_MemCore.rootSuperHeaps))
      == NULL) {
	return NULL;
    }
    memSuperHeapConstructor(pSuper, name, objectSize);
    return pSuper;
}

void
memSuperHeapDestroy(pSuper)
    MEMSuperHeap	*pSuper;
{
    memSuperHeapDestructor(pSuper);
    memSuperHeapObFree(&_MemCore.rootSuperHeaps, (TOPPtr)pSuper);
}

TOPLogical
memSuperHeapObValidB(pSuper, pOb)
    MEMSuperHeap	*pSuper;
    TOPPtr		pOb;
{
    int			ofs;

    if ( memPoolObValidB( &pSuper->pool, pOb, &ofs) == TOP_FALSE )
	return TOP_FALSE;
    if ((ofs % pSuper->objectSize) != 0)
	return TOP_FALSE;
    return TOP_TRUE;
}

TOPLogical
memSuperHeapObFreeB(pSuper, pOb)
    MEMSuperHeap	*pSuper;
    TOPPtr		pOb;
{
    MEMPoolFree		*pFree;
    MEMPoolFree		*pFreeOb = (MEMPoolFree*) pOb;

    for (pFree=pSuper->freelist; pFree != NULL; pFree=pFree->next) {
	if (pFree == pFreeOb)
	    return TOP_TRUE;
    }
    return TOP_FALSE;
}


TOPPtr
memSuperHeapObAlloc(pSuper)
    MEMSuperHeap	*pSuper;
{
    TOPPtr		pOb;
    TOPPtr		next;
    MEMPoolSeg		*pSeg;
    
    if (pSuper->freelist != NULL) {
	MEM_POOL_FREE_POP(&pSuper->freelist, &pOb);
    } else {
	pOb = memPoolObTryAlloc(&pSuper->pool, pSuper->objectSize);
	if (pOb == NULL) {
	    pSeg = _memPoolSegAlloc(&pSuper->pool, pSuper->objectSize);
	    if (pSeg == NULL) {
		return NULL;
	    }
	    pOb = pSeg->cur;
	    pSeg->cur += pSuper->objectSize;
	    /*SUPPRESS 26*/ /*SUPPRESS 29*/ /*store & cmp bad ptr*/
	    for ( ; (next=(pSeg->cur + pSuper->objectSize)) < pSeg->end;
	      pSeg->cur = next) {
		MEM_POOL_FREE_PUSH_F(&pSuper->freelist, &pSeg->cur,
		  MEM_POOL_MAGIC_WASTED);
		++(pSuper->totCrams);
	    }
	}
    }
    ((MEMPoolFree*)pOb)->magic = MEM_POOL_MAGIC_ALLOC;
    ++(pSuper->totAllocs);
    return pOb;
}

TOPStatus
memSuperHeapObFree(pSuper, pOb)
    MEMSuperHeap	*pSuper;
    TOPPtr		pOb;
{
    if (((MEMPoolFree*)pOb)->magic == MEM_POOL_MAGIC_FREE) {
	/* ok, this could happen purely by chance, lets make sure */
	if (memSuperHeapObFreeB(pSuper, pOb)) {
	    topWarnMsg("Duplicate object free (SH %s, size %d, ptr %x)",
	      pSuper->name, pSuper->objectSize, pOb);
	    ++(_MemCore.numDupFreeReal);
	    return -1;
	} else {
	    /* false alarm */
	    ++(_MemCore.numDupFreeFake);
	}
    }
#ifdef DO_IT_SAFE
    if (!memSuperHeapObValidB(pSuper, pOb)) {
	topWarnMsg("Invalid object free (SH %s, size %d, ptr %x)",
	  pSuper->name, pSuper->objectSize, pOb);
	++(_MemCore.numBadFree);
	return -1;
    }
#endif
    MEM_POOL_FREE_PUSH_F(&pSuper->freelist, &pOb, MEM_POOL_MAGIC_FREE);
    ++(pSuper->totFrees);
    return 0;
}

void
memSuperHeapStatsCalc(pSuper, pStats)
    MEMSuperHeap	*pSuper;
    MEMSuperHeapStats	*pStats;
{
    MEMPoolFree		*pFree;

    memPoolStatsCalc(&pSuper->pool, &pStats->pool);
    pStats->numHeaps = 0;
    pStats->totSubAllocs = pStats->totSubFrees = 0;
    DLMAC_LOOP_FORW_BEGIN(&pSuper->headHeapChain, MEMHeap*, pHeap) {
	++(pStats->numHeaps);
	pStats->totSubAllocs += pHeap->numAllocs;
	pStats->totSubFrees += pHeap->numFrees;
    } DLMAC_LOOP_FORW_END();
    pStats->name = pSuper->name;
    pStats->objectSize = pSuper->objectSize;

    pStats->numDead = 0;
    pStats->numWasted = 0;
    for (pFree=pSuper->freelist; pFree != NULL; pFree=pFree->next) {
	(pFree->magic == MEM_POOL_MAGIC_WASTED) 
	  ? ++(pStats->numWasted) : ++(pStats->numDead);
    }
    pStats->numLive = pSuper->totAllocs - pSuper->totFrees;
    pStats->totAllocs = pSuper->totAllocs;
    pStats->totFrees = pSuper->totFrees;
    pStats->totUsedFrees = pStats->totFrees - pStats->numDead;
    pStats->totCrams = pSuper->totCrams;
    pStats->totUsedCrams = pStats->totCrams - pStats->numWasted;

    pStats->mem.live.bytes = pStats->numLive * pStats->objectSize;
    pStats->mem.dead.bytes = pStats->numDead * pStats->objectSize;
    pStats->mem.wasted.bytes = pStats->pool.mem.wasted.bytes
      + pStats->numWasted * pStats->objectSize;
    pStats->mem.overhead.bytes = pStats->pool.mem.overhead.bytes 
      + (sizeof(*pSuper) - sizeof(pSuper->pool))
      + sizeof(MEMHeap) * pStats->numHeaps;
    memStdByteStatsCalc(&pStats->mem, 0);
}

void
memSuperHeapStatsPrint(pStats)
    MEMSuperHeapStats	*pStats;
{
    printf("    SuperHeap ``%s''\n", pStats->name);
    if (pStats->numHeaps)
	printf("\tAttached heaps: %d\n", pStats->numHeaps);
    printf("\tObjects: %d bytes each, %d live, %d dead, %d wasted\n",
      pStats->objectSize,
      pStats->numLive, pStats->numDead, pStats->numWasted);
    printf("\tLifetime: %d allocates, %d reused, %d frees\n",
      pStats->totAllocs, pStats->totUsedFrees, pStats->totFrees);
    printf("\t          %d crams, %d used crams\n",
      pStats->totCrams, pStats->totUsedCrams);
    memPoolStatsPrint(&pStats->pool);
    memStdByteStatsPrint("Memory usage", &pStats->mem);
}

void
memSuperHeapStatsShow(pSuper)
    MEMSuperHeap	*pSuper;
{
    MEMSuperHeapStats	stats;

    memSuperHeapStatsCalc(pSuper, &stats);
    memSuperHeapStatsPrint(&stats);
}



/*****************************************************************************
 *
 *		MEMHeap functions
 *
 *****************************************************************************/

void
memHeapConstructor(pHeap, name, pSuper)
    MEMHeap		*pHeap;
    char		*name;
    MEMSuperHeap	*pSuper;
{
    memset((char*)pHeap, 0, sizeof(*pHeap));
    pHeap->name = name == NULL ? "???" : name;
    pHeap->super = pSuper;
    DL_INSERT_BEFORE(&pSuper->headHeapChain, &pHeap->chain);
    ++(_MemCore.numHeaps);
}

void
memHeapDestructor(pHeap)
    MEMHeap		*pHeap;
{
    if (pHeap->numAllocs != pHeap->numFrees) {
    	topWarnMsg("heap %s (SH %s) destructed when not empty (%d vs %d)",
	  pHeap->name, pHeap->super->name,
	  pHeap->numAllocs, pHeap->numFrees);
    }
    DL_REMOVE(&pHeap->chain); /* remove from pSuper->headHeapChain */
    --(_MemCore.numHeaps);
}

MEMHeap*
memHeapCreate(name, pSuper)
    char		*name;
    MEMSuperHeap	*pSuper;
{
    MEMHeap		*pHeap;

    MEM_ASSERT_INITED();
    pHeap = (MEMHeap*) memSuperHeapObAlloc(&_MemCore.rootHeaps);
    memHeapConstructor(pHeap, name, pSuper);
    return pHeap;
}

void
memHeapDestroy(pHeap)
    MEMHeap		*pHeap;
{
    memHeapDestructor(pHeap);
    memSuperHeapObFree(&_MemCore.rootHeaps, (TOPPtr)pHeap);
}

TOPPtr
memHeapObAlloc(pHeap)
    MEMHeap		*pHeap;
{
    TOPPtr		pOb;
    
    pOb = memSuperHeapObAlloc(pHeap->super);
    ++(pHeap->numAllocs);
    return pOb;
}

TOPStatus
memHeapObFree(pHeap, pOb)
    MEMHeap		*pHeap;
    TOPPtr		pOb;
{
    if (memSuperHeapObFree(pHeap->super, pOb) < 0) {
	topWarnMsg("Free on heap %s (SH %s, ptr %x) failed", 
	  pHeap->name, pHeap->super->name, pOb);
	return -1;
    }
    ++(pHeap->numFrees);
    return 0;
}

char*
memHeapGetName(pHeap)
    MEMHeap		*pHeap;
{
    return pHeap->name;
}

void
memHeapStatsCalc(pHeap, pStats)
    MEMHeap		*pHeap;
    MEMHeapStats	*pStats;
{
    pStats->name = pHeap->name;
    pStats->allocs.bytes = pHeap->numAllocs;
    memByteStatCalc(&pStats->allocs, pHeap->super->totAllocs);
    pStats->frees.bytes = pHeap->numFrees;
    memByteStatCalc(&pStats->frees, pHeap->super->totFrees);
}

void
memHeapStatsPrint(pStats)
    MEMHeapStats	*pStats;
{
    
    printf("\t\t%-15.15s %9d(%3d%%) %9d(%3d%%)\n",
      pStats->name,
      pStats->allocs.bytes, pStats->allocs.percent,
      pStats->frees.bytes, pStats->frees.percent);
}


/*****************************************************************************
 *
 *		Convience functions that dont fit elsewhere
 *
 *****************************************************************************/

TOPStatus
memHeapBuild(name, objectSize, ppSuper, ppHeap)
    char		*name;
    unsigned		objectSize;
    MEMSuperHeap	**ppSuper;
    MEMHeap		**ppHeap;
{
    if (*ppSuper == NULL) {
	if ((*ppSuper = memSuperHeapCreate(name, objectSize)) == NULL)
	    return -1;
    }
    if ((*ppHeap = memHeapCreate(name, *ppSuper)) == NULL)
	return -1;
    return 0;
}
