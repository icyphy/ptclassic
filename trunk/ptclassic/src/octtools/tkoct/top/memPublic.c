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

static
MEMSuperHeap**
_memPublicSuperHeapFindSlot(objectSize)
    unsigned		objectSize;
{
    int			index;
    MEMSuperHeap	**supers = _MemCore.publicSuperHeaps;
    int			numLin = _MemCore.publicSuperHeapNumLinear;
    int			numTot = _MemCore.publicSuperHeapNumTotal;
    unsigned		tblLen;

    if (supers == NULL) {
	numTot = numLin;
	tblLen = numTot*sizeof(MEMSuperHeap*);
        supers = (MEMSuperHeap**) malloc(tblLen);
        if (supers == NULL) {
	    memMallocFail( tblLen, "Public SH table");
	    return NULL;
	}
        memset((char*)supers, 0, tblLen);
        _MemCore.publicSuperHeaps = supers;
        _MemCore.publicSuperHeapNumTotal = numTot;
    }
    index = objectSize <= 8 ? 0 : (objectSize>>2)-2;
    if (index < numLin)
	return &supers[index];
    for (index=numLin; index < numTot; index++) {
	if (supers[index] == NULL || supers[index]->objectSize == objectSize)
	    return &supers[index];
    }
    tblLen = numTot*sizeof(MEMSuperHeap*);
    supers = (MEMSuperHeap**) realloc((char*)supers, 2*tblLen);
    if (supers == NULL) {
	memMallocFail( 2*tblLen, "Public SH table (realloc)");
	return NULL;
    }
    memset(((char*)supers)+tblLen, 0, tblLen);
    _MemCore.publicSuperHeaps = supers;
    _MemCore.publicSuperHeapNumTotal = 2*numTot;
    return &supers[numTot];
}

static
MEMSuperHeap*
_memPublicSuperHeapGet(objectSize)
    unsigned		objectSize;
{
    MEMSuperHeap	**ppSuper;

    ppSuper = _memPublicSuperHeapFindSlot(objectSize);
    if (ppSuper == NULL) {
	return NULL;
    }
    if (*ppSuper == NULL) {
	*ppSuper = memSuperHeapCreate("public", objectSize);
        if (*ppSuper == NULL) {
	    return NULL;
	}
    }
    return *ppSuper;
}

MEMHeap*
memPublicHeapCreate(name, objectSize)
    char		*name;
    unsigned		objectSize;
{
    MEMSuperHeap	*pSuper;
    MEMHeap		*pHeap;

    if ((pSuper = _memPublicSuperHeapGet(objectSize)) == NULL) {
	return NULL;
    }
    if ((pHeap = memHeapCreate(name, pSuper)) == NULL) {
	/* there is no need to unget the superHeap */
	return NULL;
    }
    return pHeap;
}
