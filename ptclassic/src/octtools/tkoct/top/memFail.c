/* 
    memFail.c :: MEM SubPackage :: TOP Library
    This is a stand-alone part of the mem package.  It handles reporting
    allocation failures to the user.

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

#define MEM_STDERR_FD	2

static MEMMallocFailFunc *_MemMallocFailFunc = NULL;

MEMMallocFailFunc*
memSetMallocFailFunc( MEMMallocFailFunc *func) {
    MEMMallocFailFunc	*oldfunc;

    oldfunc = _MemMallocFailFunc;
    _MemMallocFailFunc = func;
    return oldfunc;
}

static void
_memMallocFailDefault( allocSize, where)
    unsigned		allocSize;
    char		*where;
{
    char		*str;

    str = "\n\n>>>>>>>>>>>   Out of memory!!!   <<<<<<<<\nAllocation for ";
      write( MEM_STDERR_FD, str, strlen(str));
    str = where;
      write( MEM_STDERR_FD, str, strlen(str));
    str = " failed.\n";
      write( MEM_STDERR_FD, str, strlen(str));
 
    /* this will prob. fail */
    fprintf( stderr, "Allocation of %u bytes for %s failed.\n",
      allocSize, where);

    fflush( stderr);

    abort();
}

void
memMallocFail( allocSize, where)
    unsigned		allocSize;
    char		*where;
{
    if (_MemMallocFailFunc == NULL) {
	_memMallocFailDefault( allocSize, where);
    } else {
	(*(_MemMallocFailFunc))( allocSize, where);
    }
}
