#if !defined(_MEMCORE_H_)
#define _MEMCORE_H_ 1
/* 
    RCS: $Header$
    Author: Kennard White

    Copyright (c) 1990-93 The Regents of the University of California.
    All rights reserved.  See the file "./copyright.h" for full copyright
    notice, limitation of liability, and disclaimer of warranty provisions.
#include "copyright.h"				COPYRIGHTENDKEY
*/

struct _MEMPoolSeg {
    DLElem		chain;		/* must be first */
    TOPPtr		start;		/* start of allocation */
    TOPPtr		cur;		/* one past last allocated */
    TOPPtr		end;		/* one past end of segment */
};
#define MEM_POOLSEG_START(pSeg) ((pSeg)->start)


struct _MEMPool {
    DLElem		chainHead;
    unsigned		poolsegSize;
};

struct _MEMPoolFree {
    MEMPoolFree		*next;
    int			magic;
};

#define MEM_POOL_MAGIC_ALLOC	(0)
#define MEM_POOL_MAGIC_FREE	(0xffff3d56)
#define MEM_POOL_MAGIC_WASTED	(0xffff3d59)

#define MEM_POOL_FREE_PUSH(ppListHead, ppOb) (			\
    ( (*(MEMPoolFree**)(ppOb))->next = (*(ppListHead)) ),	\
    ( (*(ppListHead)) = (*(MEMPoolFree**)(ppOb)) )		\
)

#define MEM_POOL_FREE_PUSH_F(ppListHead, ppOb, f) (		\
	( (*(MEMPoolFree**)(ppOb))->magic = (f) ),     		\
	MEM_POOL_FREE_PUSH((ppListHead), (ppOb))                \
)

#define MEM_POOL_FREE_POP(ppListHead, ppOb) (			\
    ( (*(MEMPoolFree**)(ppOb)) = (*(ppListHead)) ),		\
    ( (*(ppListHead)) = (*(ppListHead))->next ),		\
    ( (*(MEMPoolFree**)(ppOb))->next = NULL )			\
)

struct _MEMStack {
    DLElem		chain;		/* global chain of stacks */
    char		*name;
    MEMPool		pool;
    unsigned		totAllocs;
    unsigned		totAllocBytes;
    unsigned		totFrees;
    unsigned		totFreeBytes;
};

struct _MEMSuperHeap {
    DLElem		chain;		/* global chain of super heaps */
    DLElem		headHeapChain;	/* chain of heaps attach to this one */
    char		*name;
    unsigned		objectSize;
    MEMPool		pool;
    MEMPoolFree		*freelist;
    unsigned		totAllocs;
    unsigned		totCrams;
    unsigned		totFrees;
};

#define MEM_SUPERHEAP_MINSIZE 8		/* must be > sizeof(MEMPoolFree) */

struct _MEMHeap {
    DLElem		chain;		/* of heaps in super heap */
    char		*name;
    MEMSuperHeap	*super;
    unsigned		numAllocs;
    unsigned		numFrees;
};

/* typedef is in mem.h */
struct _MEMCore {
    DLElem		headPoolSegSegs;
    MEMSuperHeap	rootPoolSegs;
    unsigned		numStacks;
    DLElem		headStacks;
    MEMSuperHeap	rootStacks;
    unsigned		numSuperHeaps;
    DLElem		headSuperHeaps;
    MEMSuperHeap	rootSuperHeaps;
    unsigned		numHeaps;
    MEMSuperHeap	rootHeaps;
    MEMSuperHeap	**publicSuperHeaps;
    unsigned		publicSuperHeapNumLinear;
    unsigned		publicSuperHeapNumTotal;
    unsigned		numDupFreeReal;
    unsigned		numDupFreeFake;
    unsigned		numBadFree;
    unsigned		bytesMalloc;
    unsigned		bytesFree;
};


/* memCore.c */
extern MEMCore		 _MemCore;
extern TOPLogical	 _MemInitedB;
#define MEM_ASSERT_INITED()	( _MemInitedB ? 0 : (_memAssertInited(), 0) )
extern void		_memAssertInited();

/* memPool.c */
extern MEMPoolSeg*	_memPoolSegAlloc( MEMPool *pPool, unsigned allocSize);
#endif /* _MEMCORE_H_ */
