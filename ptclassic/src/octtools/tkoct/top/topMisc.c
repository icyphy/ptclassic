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
#include "topMem.h"

extern int sys_nerr;
extern char *sys_errlist[];

/*
 * Some global variables may or may not be useful.
 */

/*
 * XServerFix requests that a work around be used for 
 * a SparcStation X server bug with XDrawLines().
 */
/*global*/ TOPLogical TOPXServerFixB = TOP_FALSE;

char* 
topStrError(error)
    int			error;
{
    static char		defaultMsg[100];

    if ( error < 0 || error >= sys_nerr ) {
	sprintf(defaultMsg, "unknown system error (%d)", error);
	return defaultMsg;
    }
    return sys_errlist[error];
}

char*
topBasename(char *path) {
    char		*base;

    if ( path == NULL || path[0] == '\0' ) {
	return "UnknownName";
    }
    if ( (base=strrchr(path,'/')) != NULL )	return base+1;
    return path;
}

void
topSetEnv(char *name, char *value) {
    putenv(memStrSaveFmt("%s=%s",name,value));
}

/**
    Returns a pointer to first {subpath} within {path}.
**/
char*
topFindSubPath( char *path, char *subpath) {
    char	*s = path;
    int		len = strlen(subpath);

    if ( *s == '/' )	++s;
    while ( TOP_TRUE ) {
	if ( strncmp(s,subpath,(unsigned)len)==0 
	  && (s[len]=='\0' || s[len]=='/') )
	    return s;
	if ( (s=strchr(s,'/')) != NULL )	++s;
	else					break;
    }
    return NULL;
}
