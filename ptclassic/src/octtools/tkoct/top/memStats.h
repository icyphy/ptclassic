#if !defined(_MEMSTATS_H_)
#define _MEMSTATS_H_ 1
/* 
    RCS: $Header$
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

typedef struct _MEMByteStat {
    unsigned		bytes;
    unsigned		percent;
} MEMByteStat;

typedef struct _MEMStdByteStats {
    MEMByteStat		live;
    MEMByteStat		dead;
    MEMByteStat		wasted;
    MEMByteStat		overhead;
    MEMByteStat		total;
} MEMStdByteStats;

typedef struct _MEMPoolStats {
    unsigned		numSegs;
    MEMStdByteStats	mem;
} MEMPoolStats;

typedef struct _MEMSuperHeapStats {
    char		*name;
    MEMPoolStats	pool;
    unsigned		numHeaps;
    unsigned		objectSize;
    unsigned		numLive;
    unsigned		numDead;
    unsigned		numWasted;
    unsigned		totAllocs;
    unsigned		totReclaims;
    unsigned		totFrees;
    unsigned		totUsedFrees;
    unsigned		totCrams;
    unsigned		totUsedCrams;
    MEMStdByteStats	mem;
    unsigned		totSubAllocs;
    unsigned		totSubFrees;
} MEMSuperHeapStats;

typedef struct _MEMHeapStats {
    char		*name;
    MEMByteStat		allocs;		/* mis-use of byteStat */
    MEMByteStat		frees;
} MEMHeapStats;

typedef struct _MEMStackStats {
    char		*name;
    MEMPoolStats	pool;
    MEMStdByteStats	mem;
} MEMStackStats;

typedef struct _MEMCoreStats {
    unsigned		totSuperHeaps;
    unsigned		totHeaps;
    unsigned		totStacks;
    MEMStdByteStats	heapMem;
    MEMStdByteStats	stackMem;
    MEMStdByteStats	internalMem;
    MEMByteStat		missingMem;
    MEMByteStat		totalMem;
    unsigned		numDupFreeReal;
    unsigned		numDupFreeFake;
    unsigned		numBadFree;
} MEMCoreStats;

/* memCore.c */
extern void		memByteStatCalc( MEMByteStat *pStat, unsigned total);
extern void		memByteStatPrint( char *name, MEMByteStat *pStat);
extern void		memByteStatPrintTotal( char *name, MEMByteStat *pStat);
extern void		memStdByteStatsCalc(MEMStdByteStats *pStats, unsigned total);
extern void		memStdByteStatsPrint( char *name, MEMStdByteStats *pStats);

extern void		memCoreStatsCalc( MEMCoreStats *pStats);
extern void		memCoreStatsPrint( MEMCoreStats *pStats);
extern void		memCoreStatsShow();

extern void		memPoolStatsCalc( MEMPool *pPool, MEMPoolStats *pStats);
extern void		memPoolStatsPrint( MEMPoolStats *pStats);

extern void		memSuperHeapStatsCalc( MEMSuperHeap *pSuper, 
			  MEMSuperHeapStats *pStats);
extern void		memSuperHeapStatsPrint( MEMSuperHeapStats *pStats);
extern void		memSuperHeapStatsShow( MEMSuperHeap *pSuper);

extern void		memHeapStatsCalc( MEMHeap *pHeap, MEMHeapStats *pStats);
extern void		memHeapStatsPrint( MEMHeapStats *pStats);

extern void		memStackStatsCalc( MEMStack *pStack, MEMStackStats *pStats);
extern void		memStackStatsPrint( MEMStackStats *pStats);
extern void		memStackStatsShow( MEMStack *pStack);

#endif /* _MEMSTATS_H_ */
