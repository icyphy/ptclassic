#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/*
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
#include "port.h"
#include "utility.h"

#ifdef SOURCE_HEADER
/*
 *    RCS Control Information:
 *    $Author$				
 *    $Date$
 *    $Revision$		
 *
 *    Modification History
 *
 *    002 George Carvalho (13 June 91)
 *        Changed to take advantage of setenv() and unsetenv() procedures
 *        provided under ULTRIX 4.
 *
 *    001 Andrea Casotto
 *        Converted from c++ for octtools utilities.
 */
#endif /* SOURCE_HEADER */

extern char** environ;
 
/*
 * ULTRIX 4.x defined setenv() and unsetenv(): take advantage of that.
 */
#if !defined(ultrix4)

/* ---------- Local Procedures ---------- */
static void setenv_init();
static int  setenv_envlen();
static int  envfind();


static int setenv_envlen( environ )
    char** environ;
{
    int envlen = 0;
    while ( environ[envlen++] != 0 ) ;
    return envlen;
}


static void setenv_init() 
{
    static copiedP = 0;
    if ( ! copiedP ) {
	int envlen = setenv_envlen( environ );
	char** newenviron = ALLOC( char*, envlen + 1 );
	char** ep;
	char** nep;
	for (ep=environ, nep=newenviron; *ep != NULL; ep++, nep++ ) {
	    *nep = util_strsav( *ep );
	}
	*nep = NULL;
	environ = newenviron;
	copiedP = 1;
    }
}   


static int envfind( name )
    char* name;
{
    char* equals;
    int i = 0;
    int equalP;

    for ( i = 0; environ[i] != 0 ; i++ ) {
	char* e = environ[i];
	equals = strchr(e, '=');
	*equals = '\0';				/* Terminate name */
	equalP =  ( ! strcmp(e, name) );
	*equals = '=';				/* Restore '=' symbol */
	if ( equalP ) return i;
    }
    return -1;					/* Not found. */
}
#endif /* ultrix4 */

/*
 *    setenv - set an environment variable.
 *
 *    return nothing, just do it.
 */
#if defined(ultrix4) /* { */
void
util_setenv( name, value )
    char* name;
    char* value;
{
	(void) setenv(name, value, (int) 1);
}
#else  /* } else { ultrix4 */
void
util_setenv( name, value )
    char* name;
    char* value;
{
    char** ep;
    char** nep;
    char** newenviron;
    char* newvalue;
    int namelen, valuelen, envlen;
    int i;			/* Index of variable to be modified. */

    setenv_init();

    namelen = strlen(name);
    valuelen = strlen(value);
    newvalue = ALLOC( char, namelen+valuelen+2);

    strcpy(newvalue, name);
    newvalue[namelen] = '=';
    strcpy(&newvalue[namelen+1], value);

    /*
     *    Step through all of the environment variables
     */
    i = envfind( name );
    if ( i >= 0 ) {				/* Found */
	FREE( environ[i] );			/*  */
	environ[i] = newvalue;			/* Replace */
	return;					/*  */
    }

    envlen = setenv_envlen(environ);
    newenviron = ALLOC( char*, envlen+2);

    for (ep=environ, nep=newenviron; *ep != NULL; ep++, nep++)
      *nep = *ep;
    *nep++ = newvalue;
    *nep = NULL;

    FREE(environ);
    environ = newenviron;
}
#endif /* } ultrix4 */

/*
 *    unsetenv - unset an environment variable
 *
 *    return nothing, just do it.
 */
#if defined(ultrix4) /* { */
void 
util_unsetenv(name)
char *name;			/* unset this name	*/
{
	unsetenv(name);
}
#else  /* } else { ultrix4 */
void 
util_unsetenv(name)
char *name;			/* unset this name	*/
{
    char** ep;
    int i;

    setenv_init();

    /*
     *    Step through all of the environment variables
     */
    i = envfind( name );

    if ( i >= 0 ) {
	ep = environ + i;
	FREE(*ep);
	do {
	    *ep = ep[1];
	    ep++;
	} while (*ep != NULL);
    }
}
#endif /* } ultrix4 */

void util_printenv( fp )
    FILE* fp;
{
    int i = 0;
    while ( environ[i] != 0 ) {
	(void)fprintf( fp, "%s\n", environ[i] );
	i++;
    }
}


