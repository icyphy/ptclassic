/* 
    memFail.c :: MEM SubPackage :: TOP Library
    This is a stand-alone part of the mem package.  It handles reporting
    allocation failures to the user.

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
