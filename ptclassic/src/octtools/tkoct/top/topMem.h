#if !defined(_TOPMEM_H_)
#define _TOPMEM_H_ 1
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


/*****************************************************************************
 *
 *		Types visible to the application
 *
 *****************************************************************************/


typedef struct _MEMPoolSeg	MEMPoolSeg,	*MEMPoolSegHandle;
typedef struct _MEMPool		MEMPool,	*MEMPoolHandle;
typedef struct _MEMPoolFree	MEMPoolFree,	*MEMPoolFreeHandle;
typedef struct _MEMStack	MEMStack,	*MEMStackHandle;
typedef struct _MEMSuperHeap	MEMSuperHeap,	*MEMSuperHeapHandle;
typedef struct _MEMHeap		MEMHeap,	*MEMHeapHandle;
typedef struct _MEMCore		MEMCore,	*MEMCoreHandle;

typedef void (MEMMallocFailFunc) ARGS((unsigned alloc_size, char *where));

/*****************************************************************************
 *
 *		Type-casting macros
 *
 *****************************************************************************/

#define MEM_ALLOC(t)		( (t*)memMalloc(sizeof(t)) )
#define MEM_ALLOC_BYTES(t,b)	( (t*)memMalloc((b)) )
#define MEM_ALLOC_VEC(t,n)	( (t*)memCalloc(n,sizeof(t)) )
#define MEM_REALLOC_VEC(p,t,n)	( (t*)realloc((char*)(p), sizeof(t)*(n)) )
#define MEM_FREE(x)		( (x) == 0 ? 0 : (memFree((TOPMallocPtr)x),0) )

#define MEM_CLEAR(p)		( memset((char*)(p), 0, sizeof(*p)),0 )
#define MEM_CLEAR_BYTES(p,b)	( memset((char*)(p), 0, (b)),0 )

/*****************************************************************************
 *
 *		Exports
 *
 *****************************************************************************/

/* memCore.c */
extern TOPStatus	memInitialize(/* acp, avp */);

/* memFail.c */
extern MEMMallocFailFunc* memSetMallocFailFunc ARGS((MEMMallocFailFunc *func));
extern void		memMallocFail ARGS((unsigned allocSize, char *where));

/* memMalloc.c */
extern TOPMallocPtr	memMalloc ARGS((unsigned size));
extern TOPMallocPtr	memCalloc ARGS((unsigned nelem, unsigned elsize));
extern TOPMallocPtr	memRealloc ARGS((TOPMallocPtr ptr, unsigned size));
extern void		memFree ARGS((TOPMallocPtr ptr));
extern char*		memStrSave ARGS((char *s));
extern char*		memStrSaveVa ARGS_VA((char *fmt, va_list args));
extern char*		memStrSaveFmt ARGS((char *fmt, ...));


/* memPool.c */
extern void		memPoolConstructor ARGS((MEMPool *pPool,
			  unsigned initialPoolSize));
extern void		memPoolDestructor ARGS((MEMPool *pPool));
extern TOPPtr		memPoolObAlloc ARGS((MEMPool *pPool, unsigned allocSize));
extern TOPPtr		memPoolObTryAlloc ARGS((MEMPool *pPool, unsigned allocSize));
extern TOPLogical	memPoolObValidB ARGS((MEMPool *pPool, TOPPtr pOb, int *pOfs));


/* memHeap.c */
extern MEMSuperHeap*	memSuperHeapCreate ARGS((char *name, unsigned objectSize));
extern void		memSuperHeapDestroy ARGS((MEMSuperHeap *pSuper));
extern TOPPtr		memSuperHeapObAlloc ARGS((MEMSuperHeap *pSuper));
extern TOPStatus	memSuperHeapObFree ARGS((MEMSuperHeap *pSuper,
			  TOPPtr pOb));
extern void		memSuperHeapConstructor ARGS((MEMSuperHeap *pSuper,
			  char *name, unsigned objectSize));
extern void		memSuperHeapDestructor ARGS((MEMSuperHeap *pSuper));
extern TOPLogical	memSuperHeapObFreeB ARGS((MEMSuperHeap *pSuper, 
			  TOPPtr pOb));
extern TOPLogical	memSuperHeapObValidB ARGS((MEMSuperHeap *pSuper, 
			  TOPPtr pOb));

extern MEMHeap*		memHeapCreate ARGS((char *name, MEMSuperHeap *pSuper));
extern TOPStatus	memHeapBuild ARGS((char *name, unsigned objectSize, 
			  MEMSuperHeap **ppSuper, MEMHeap **ppHeap));
extern void		memHeapDestroy ARGS((MEMHeap *pHeap));
extern TOPPtr		memHeapObAlloc ARGS((MEMHeap *pHeap));
extern TOPStatus	memHeapObFree ARGS((MEMHeap *pHeap, TOPPtr pOb));
extern void		memHeapConstructor ARGS((MEMHeap *pHeap, char *name, MEMSuperHeap *pSuper));
extern void		memHeapDestructor ARGS((MEMHeap *pHeap));
extern char*		memHeapGetName ARGS((MEMHeap *pHeap));


/* memPublic.c */
extern MEMHeap*		memPublicHeapCreate ARGS((char *name,
			  unsigned objectSize));

/* memStack.c */
extern MEMStack*	memStackCreate ARGS((char *name));
extern void		memStackDestroy ARGS((MEMStack *pStack));
extern TOPPtr		memStackObAlloc ARGS((MEMStack *pStack, unsigned size));
extern void		memStackConstructor ARGS((MEMStack *pStack, char *name));
extern void		memStackDestructor ARGS((MEMStack *pStack));
extern TOPLogical	memStackObValidB ARGS((MEMStack *pStack, TOPPtr pOb));

/* memTcl.c */
extern int		memRegisterCmds ARGS_TCL((Tcl_Interp *ip));

#endif /* _TOPMEM_H_ */
