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

#include "dlMacros.h"
#include "topMem.h"
#include "memCore.h"
#include "memStats.h"


/*****************************************************************************
 *
 *		MEMStack functions
 *
 *****************************************************************************/

#define MEM_STACK_DEFSIZE 256

void
memStackConstructor(pStack, name)
    MEMStack		*pStack;
    char		*name;
{
    memset((char*)pStack, 0, sizeof(*pStack));
    pStack->name = name;
    memPoolConstructor(&pStack->pool, MEM_STACK_DEFSIZE);
    ++(_MemCore.numStacks);
    DL_INSERT_BEFORE(&_MemCore.headStacks, &pStack->chain);
}

void
memStackDestructor(pStack)
    MEMStack		*pStack;
{
    memPoolDestructor(&pStack->pool);
    if (pStack->chain.next != NULL) {
    	DL_REMOVE(&pStack->chain);    /* remove from _MemCore.headStacks */
	DL_CLEAR(&pStack->chain);
    	--(_MemCore.numStacks);
    }
}

MEMStack*
memStackCreate(name)
    char		*name;
{
    MEMStack		*pStack;

    MEM_ASSERT_INITED();
    if ((pStack = (MEMStack*)memSuperHeapObAlloc(&_MemCore.rootStacks))
      == NULL) {
	return NULL;
    }
    memStackConstructor(pStack, name);
    return pStack;
}

void
memStackDestroy(pStack)
    MEMStack		*pStack;
{
    memStackDestructor(pStack);
    memSuperHeapObFree(&_MemCore.rootStacks, (TOPPtr)pStack);
}

TOPLogical
memStackObValidB(pStack, pOb)
    MEMStack		*pStack;
    TOPPtr		pOb;
{
    return memPoolObValidB( &pStack->pool, pOb, NULL);
}

/*
 * XXX: alignment is a big problem here.  For now, force all requests
 * to in 8-byte blocks.  This assures that the next request will be 8-byte
 * aligned.  This is painful, but easy to implement.
 */
TOPPtr
memStackObAlloc(pStack, allocSize)
    MEMStack		*pStack;
    unsigned		allocSize;
{
    TOPPtr		pOb;
    
    pOb = memPoolObAlloc( &pStack->pool, ((allocSize-1) | 7)+1);
    if ( pOb != NULL ) {
	++(pStack->totAllocs);
	pStack->totAllocBytes += allocSize;
    }
    return pOb;
}

void
memStackStatsCalc(pStack, pStats)
    MEMStack		*pStack;
    MEMStackStats	*pStats;
{
    pStats->name = pStack->name;
    memPoolStatsCalc(&pStack->pool, &pStats->pool);

    pStats->mem = pStats->pool.mem;
    pStats->mem.overhead.bytes += sizeof(*pStack)-sizeof(pStack->pool);
    memStdByteStatsCalc(&pStats->mem, 0);
}

void
memStackStatsPrint(pStats)
    MEMStackStats	*pStats;
{
    printf("    Stack ``%s''\n", pStats->name);
    memStdByteStatsPrint("Memory usage", &pStats->mem);
}

void
memStackStatsShow(pStack)
    MEMStack		*pStack;
{
    MEMStackStats	stats;

    memStackStatsCalc(pStack, &stats);
    memStackStatsPrint(&stats);
}
