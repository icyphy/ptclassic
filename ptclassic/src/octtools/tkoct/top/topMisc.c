/* 
    Author: Kennard White

    Copyright (c) 1990-93 The Regents of the University of California.
    All rights reserved.  See the file "./copyright.h" for full copyright
    notice, limitation of liability, and disclaimer of warranty provisions.
#include "copyright.h"				COPYRIGHTENDKEY
*/
#if !defined(lint) && !defined(SABER)
static char rcsid[] = "$Header$";
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
