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
#include <malloc.h>
#include <stdarg.h>
#include "topStd.h"

#include "dlMacros.h"
#include "topMem.h"
#include "memCore.h"
#include "memStats.h"

TOPMallocPtr
memMalloc(size)
    unsigned		size;
{
    TOPMallocPtr		ptr;

    if ((ptr = malloc(size)) == NULL) {
	memMallocFail( size, "memMalloc");
	return NULL;
    }
    _MemCore.bytesMalloc += size;
    return ptr;
}

void
memFree(ptr)
    TOPMallocPtr		ptr;
{
   if (ptr == NULL)
	return;
#if defined(__sun__) && !defined(__GNUC__)
    if (free(ptr) == 0) {
	topWarnMsg( "Can't free memory");
    }
#else
    free(ptr);
#endif
    return;
}

TOPMallocPtr
memRealloc(ptr, size)
    TOPMallocPtr		ptr;
    unsigned		size;
{
    TOPMallocPtr		newptr;

    newptr = realloc(ptr, size);
    if (newptr == NULL) {
	memMallocFail( size, "memRealloc");
	return NULL;
    }
    return newptr;
}

TOPMallocPtr
memCalloc(nelem, elsize)
    unsigned		nelem;
    unsigned		elsize;
{
    TOPMallocPtr		ptr;

    ptr = calloc(nelem, elsize);
    if (ptr == NULL) {
	memMallocFail( nelem*elsize, "memCalloc");
	return NULL;
    }
    return (ptr);
}


char*
memStrSave( srcStr)
    char		*srcStr;
{
    unsigned		len;
    char		*dstStr;

    if ( srcStr == NULL )
	return NULL;
    len = strlen(srcStr) + 1;
    if ( (dstStr = malloc(len)) == NULL ) {
	memMallocFail( len, "memStrSave");
	return NULL;
    }
    memcpy( dstStr, srcStr, len);
    return dstStr;
}

char*
memStrSaveVa( fmt, args)
    char		*fmt;
    va_list		args;
{
    char		buf[4000];	/* XXX */

    vsprintf( buf, fmt, args);
    return memStrSave( buf);
}

/*
 * Note: Must use ANSI args style with stdarg macros on mips native cc
 */
/*VARARGS*/
char*
memStrSaveFmt( char *fmt, ...) {
    char		*str;
    va_list		args;
    va_start( args, fmt);
    str = memStrSaveVa( fmt, args);
    va_end( args);
    return str;
}
