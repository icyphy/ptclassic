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
#include <stdio.h>
#include "topStd.h"

#include "dlMacros.h"
#include "topMem.h"
#include "memCore.h"
#include "memStats.h"

extern int end;

/*global*/ TOPLogical _MemInitedB = TOP_FALSE;
/*global*/ MEMCore _MemCore;

void
_memAssertInited()
{
    if (_MemInitedB) 
	return;
    topMsg( TOP_MSG_ERR|TOP_MSG_DIE,
      "Memory package was not initialized before use; use memInitialize()");
}

static void
_memBootstrap()
{
    /*
     * WARNING: The order of initialization here is very critical
     * NOTE: these root super heaps are not linked into the global list
     * of super heaps.  The heap package special cases the name "^root...".
     */
    memset((char*)&_MemCore, 0, sizeof(_MemCore));
    DL_INIT_HEAD(&_MemCore.headPoolSegSegs);
    DL_INIT_HEAD(&_MemCore.headSuperHeaps);
    memSuperHeapConstructor(&_MemCore.rootPoolSegs, "^rootPoolSegs",
      sizeof(MEMPoolSeg));
    memSuperHeapConstructor(&_MemCore.rootSuperHeaps, "^rootSuperHeaps",
      sizeof(MEMSuperHeap));
    memSuperHeapConstructor(&_MemCore.rootHeaps, "^rootHeaps",
      sizeof(MEMHeap));
    _MemCore.publicSuperHeapNumLinear = 64;

    DL_INIT_HEAD(&_MemCore.headStacks);
    memSuperHeapConstructor( &_MemCore.rootStacks, "^rootStacks",
      sizeof(MEMStack));
}

void
memCoreStatsCalc(pStats)
    MEMCoreStats	*pStats;
{
    MEMSuperHeapStats	superStats;		
#ifdef notdef
    MEMHeapStats	heapStats;		
#endif
    MEMStackStats	stackStats;		

    memset((char*)pStats, 0, sizeof(*pStats));

    pStats->totalMem.bytes = ((unsigned)sbrk(0)) - ((unsigned)&end);

    /*
     * Go through each of the super heaps and its heaps
     */
    DLMAC_LOOP_FORW_BEGIN(&_MemCore.headSuperHeaps, MEMSuperHeap*, pSuper) {
	++(pStats->totSuperHeaps);
	memSuperHeapStatsCalc(pSuper, &superStats);
	pStats->totHeaps += superStats.numHeaps;
	pStats->heapMem.live.bytes += superStats.mem.live.bytes;
	pStats->heapMem.dead.bytes += superStats.mem.dead.bytes;
	pStats->heapMem.wasted.bytes += superStats.mem.wasted.bytes;
	pStats->heapMem.overhead.bytes += superStats.mem.overhead.bytes;
#ifdef notdef
        DLMAC_LOOP_FORW_BEGIN(&pSuper->headHeapChain, MEMHeap*, pHeap) {
	    ++(pStats->totHeaps);
        } DLMAC_LOOP_FORW_END();
#endif
    } DLMAC_LOOP_FORW_END();
    memStdByteStatsCalc(&pStats->heapMem, pStats->totalMem.bytes);

    /*
     * Go through each of the stacks
     */
    DLMAC_LOOP_FORW_BEGIN(&_MemCore.headStacks, MEMStack*, pStack) {
	++(pStats->totStacks);
	memStackStatsCalc(pStack, &stackStats);
	pStats->stackMem.live.bytes += stackStats.mem.live.bytes;
	pStats->stackMem.dead.bytes += stackStats.mem.dead.bytes;
	pStats->stackMem.wasted.bytes += stackStats.mem.wasted.bytes;
	pStats->stackMem.overhead.bytes += stackStats.mem.overhead.bytes;
    } DLMAC_LOOP_FORW_END();
    memStdByteStatsCalc(&pStats->stackMem, pStats->totalMem.bytes);

    /*
     * Add up the internal mem used by certain control structures
     */
    memSuperHeapStatsCalc(&_MemCore.rootPoolSegs, &superStats);
    pStats->internalMem.dead.bytes += superStats.mem.dead.bytes;
    pStats->internalMem.wasted.bytes += superStats.mem.wasted.bytes;
    memSuperHeapStatsCalc(&_MemCore.rootSuperHeaps, &superStats);
    pStats->internalMem.dead.bytes += superStats.mem.dead.bytes;
    pStats->internalMem.wasted.bytes += superStats.mem.wasted.bytes;
    memSuperHeapStatsCalc(&_MemCore.rootHeaps, &superStats);
    pStats->internalMem.dead.bytes += superStats.mem.dead.bytes;
    pStats->internalMem.wasted.bytes += superStats.mem.wasted.bytes;
    memSuperHeapStatsCalc(&_MemCore.rootStacks, &superStats);
    pStats->internalMem.dead.bytes += superStats.mem.dead.bytes;
    pStats->internalMem.wasted.bytes += superStats.mem.wasted.bytes;
    memStdByteStatsCalc(&pStats->internalMem, pStats->totalMem.bytes);

    /*
     * Get it all together, and find out how much is missing
     */
    pStats->missingMem.bytes = pStats->totalMem.bytes
      -( pStats->heapMem.total.bytes 
       + pStats->stackMem.total.bytes 
       + pStats->internalMem.total.bytes);
    memByteStatCalc(&pStats->missingMem, pStats->totalMem.bytes);
    memByteStatCalc(&pStats->totalMem, pStats->totalMem.bytes);

    pStats->numDupFreeReal = _MemCore.numDupFreeReal;
    pStats->numDupFreeFake = _MemCore.numDupFreeFake;
    pStats->numBadFree = _MemCore.numBadFree;
}

void
memCoreStatsPrint(pStats)
    MEMCoreStats	*pStats;
{
    char		buf[100];

    printf("Global statistics:\n");
    sprintf(buf, "Heap Memory (%d H, %d SH)", 
      pStats->totHeaps, pStats->totSuperHeaps);
    memStdByteStatsPrint(buf, &pStats->heapMem);
    sprintf(buf, "Stack Memory (%d)", pStats->totStacks);
    memStdByteStatsPrint(buf, &pStats->stackMem);
    memStdByteStatsPrint("Internal Memory", &pStats->internalMem);
    memByteStatPrintTotal("Missing Memory", &pStats->missingMem);
    memByteStatPrintTotal("Total Dynamic Core", &pStats->totalMem);
    printf("\n");
    printf("SuperHeap statistics:\n");
    printf("\tfrees: %d bad, %d dupreal, %d dupfake\n",
      pStats->numBadFree, pStats->numDupFreeReal, pStats->numDupFreeFake);
}

void
memCoreStatsShow()
{
    MEMCoreStats	coreStats;
    MEMHeapStats	heapStats;		

    printf("********  MEMCore statistics begin *********\n");
    memCoreStatsCalc(&coreStats);
    memCoreStatsPrint(&coreStats);
    printf("********  Heaps\n");
    DLMAC_LOOP_FORW_BEGIN(&_MemCore.headSuperHeaps, MEMSuperHeap*, pSuper) {
	memSuperHeapStatsShow(pSuper);
	printf("\tAttached heaps:\t\t\t  Allocs      Frees\n");
        DLMAC_LOOP_FORW_BEGIN(&pSuper->headHeapChain, MEMHeap*, pHeap) {
	    memHeapStatsCalc(pHeap, &heapStats);
	    memHeapStatsPrint(&heapStats);
        } DLMAC_LOOP_FORW_END();
    } DLMAC_LOOP_FORW_END();

    printf("********  Stacks\n");
    DLMAC_LOOP_FORW_BEGIN(&_MemCore.headStacks, MEMStack*, pStack) {
	memStackStatsShow(pStack);
    } DLMAC_LOOP_FORW_END();

    printf("********  MEMCore statistics end *********\n");
}

/*ARGSUSED*/
TOPStatus
memInitialize(acp, avp)
    int			*acp;
    char		***avp;
{
    if (_MemInitedB)
	return 0;
    _memBootstrap();
    _MemInitedB = TOP_TRUE;
    return 0;
}

/*****************************************************************************
 *
 * 		Generic statistics functions
 *
 *****************************************************************************/


void
memByteStatCalc(pStat, total)
    MEMByteStat		*pStat;
    unsigned		total;
{
    pStat->percent = ((int)total) > 0 ? (pStat->bytes * 100) / total : 0;
}

void
memByteStatPrint(name, pStat)
    char		*name;
    MEMByteStat		*pStat;
{
    printf("\t\t%-30.30s %9d(%3d%%)\n",
      name, pStat->bytes, pStat->percent);
}

void
memByteStatPrintTotal(name, pStat)
    char		*name;
    MEMByteStat		*pStat;
{
    printf("\t%-30.30s %-24s %9d(%3d%%)\n",
      name, "", pStat->bytes, pStat->percent);
}

void
memStdByteStatsCalc(pStats, total)
    MEMStdByteStats	*pStats;
    unsigned		total;
{
    unsigned		t;
    
    t = pStats->total.bytes = pStats->live.bytes + pStats->dead.bytes
      + pStats->wasted.bytes + pStats->overhead.bytes;
    memByteStatCalc(&pStats->live, t);
    memByteStatCalc(&pStats->dead, t);
    memByteStatCalc(&pStats->wasted, t);
    memByteStatCalc(&pStats->overhead, t);
    memByteStatCalc(&pStats->total, total > 0 ? total : t);
}

void
memStdByteStatsPrint(name, pStats)
    char		*name;
    MEMStdByteStats	*pStats;
{
    printf("\t%-38.38s      Bytes\n", name);
    memByteStatPrint("Live", &pStats->live);
    memByteStatPrint("Dead", &pStats->dead);
    memByteStatPrint("Wasted", &pStats->wasted);
    memByteStatPrint("Overhead", &pStats->overhead);
    memByteStatPrintTotal("        Total", &pStats->total);
}
