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
#include "copyright.h"
#include "port.h"
#include "utility.h"
#include "errtrap.h"

#define RESOLVE_MAXPATHLENGTH 1024

static int isDelimiter( c )
    char c;
/*
 * Determine if c is a delimiter for a variable.
 * Could be implemented by strpbrk() except I do not know if it 
 * is a portable call
 */
{
    if (c == '/'  || 
	c == '\0' ||
	c == '.'  ||
	c == '$'  ||
	c == '\t' ||
	c == ' '
	) return 1;
    return 0;
}

char* util_logical_expand( path )
    char* path;		/*  */
/*
 * Name resolution routine for octtools.
 * 
 * Replace all references of environment variables with
 * the corresponding values.
 * An environment variable can be referenced by $VAR or by ${VAR}
 * The returned path is statically allocated and it should not be freed.
 * ERRORS:
 * Errors are handled by errtrap.
 * EXAMPLE:
 *    given OCTTOOLS=/net/users/octtools
 *    path = "$OCTTOOLS/tech"
 *    util_logical_expand( path ) = "/net/users/octtools/tech"
 */
{
    static char buffer[8][RESOLVE_MAXPATHLENGTH];
    static int count = 0;
    static char* ov = (char*)1;	/* Value of $OCTTOOLS, if any. */
    char tmpbuf[RESOLVE_MAXPATHLENGTH];
    register char* first;
    register char* last;
    register char* tail;	/* The part of path following the variable reference */
    char  variable[256];
    int varlength;
    char* value;
    int   avoid_infinite_loops = 32; /* I do not expect too many expansions. */

    if ( ++count > 7 ) count = 0;

    if ( ov == (char*)1 ) {
	ov = getenv( "OCTTOOLS" ); /* Cache value of OCTTOOLS */
    }
    /* Speed up some normal expansions */
    if ( ov && strncmp( path, "$OCTTOOLS", 9 ) == 0 ) {
	strcpy( buffer[count], ov );
	strcat( buffer[count], path + 9 );
    } else if ( ov && strncmp( path, "${OCTTOOLS}", 11 ) == 0 ) {
	strcpy( buffer[count], ov );
	strcat( buffer[count], path + 11 );
    } else {
	strcpy( buffer[count], path );
    }

    while ((first = strchr(buffer[count],'$')) != NIL(char) ) { /* Find variable beginning. */
	if ( !avoid_infinite_loops-- ) {
	    errRaise( UTILITY_PKG_NAME, 1, "Infinite loop expanding `%s'", path );
	}
	*first = '\0';					/* Terminate head part of path. */
	if ( first[1] == '{' ) {
	    first += 2;
	    last = strchr( first, '}' );
	    if ( last == NIL( char ) ) {
		errRaise( UTILITY_PKG_NAME, 1, "Missing `}' in `%s'", path );
	    }
	    tail = last + 1;
	} else {
	    first += 1;
	    last = first;
	    while ( !isDelimiter( *last ) ) last++;
	    tail = last;
	}

	last--;						/* Go to end of variable name */

	varlength = last - first + 1;
	if ( varlength <= 0 ) {
	    errRaise( UTILITY_PKG_NAME, 1, "Illegal logical name\n\t\"%s\"", path );
	}
	strncpy( variable, first, varlength );
	variable[varlength] = '\0';			/* Terminate string. */

	value = getenv( variable );			/* Get value of the variable. */
	if ( value == NIL(char) ) {
	    errRaise(UTILITY_PKG_NAME,1,"Undefined variable `%s'\n\tin expansion of `%s'",variable,path);
	}

	strcpy( tmpbuf, tail );		/* save the tail before we lose it  */
	strcat( buffer[count], value);  /*  */
	strcat( buffer[count], tmpbuf);
    }

    return buffer[count];
}



char* util_logical_compress( path, var )
    char* path;			/* Path to modify. */
    char* var;			/* Variable name to substitute. */
{
    int lvalue;			/* Length of value string. */
    int lbuf;			/* Length of buffer. */
    int i = 0;			/* An index in path. */
    char* value;
    static char buffer[8][RESOLVE_MAXPATHLENGTH];
    static count = 0;

    value = getenv(var);	/* Value of the environment variable. */

    if ( ++count > 7 ) count = 0;

    strcpy( buffer[count], path );
    if ( value == NIL(char) ) {
	errRaise( UTILITY_PKG_NAME, 1, "Variable `%s' not defined (compressing %s)",
		 var,path );
    }
    lvalue = strlen( value );
    if ( lvalue <= 3 ) {
	errRaise( UTILITY_PKG_NAME, 1, "Variable `%s=%s' too short to be trusted",
		 var, value );
    }

    lbuf  = strlen( buffer[count] );

    while ( i + lvalue <= lbuf ) {
	if ( strncmp( buffer[count] + i, value, lvalue ) == 0 ) {
	    char tail[RESOLVE_MAXPATHLENGTH];
	    char head[RESOLVE_MAXPATHLENGTH];
	    strcpy( tail, buffer[count] + i + lvalue );
	    if ( i ) {
		strncpy( head, buffer[count], i );
	    } 
	    head[i] = '\0';
	    sprintf( buffer[count], "%s${%s}%s", head, var, tail );
	    break;
	}
	i++;
    }
    return buffer[count];
}

char* util_file_expand( path )
    char* path;
{
    char* file = util_logical_expand( path );
    return util_tilde_expand( file );
}

#define TEST

#ifdef TEST

jmp_buf env;
void trap( name, code, message )
     char* name;
     int code;
     char* message;
{
  printf( "\t%s\n", message );
  longjmp( env, 1 );
}

void test_resolve( )
{
  static char* paths[] = {
    "$OCTTOOLS",
    "${OCTTOOLS}",
    "$OCTTOOLS/foo",
    "${OCTTOOLS}/foo",
    "/net/$OCTTOOLS/foo",
    "/net/${OCTTOOLS}/foo",
    "/net/users/$OCTTOOLS",
    "/net/users/${OCTTOOLS}",
    
    "/net/users/octtools",
    "ERRORS",
    "/net/$OOOOO/foo",
    "/net/${OCTTOOLS/foo",
    "/net/$OCTTOOLS}/foo",
    "/net/$ILLEGAL",
    "END"
    };
  int i = 0;

  util_setenv( "OCTTOOLS", "octtoolsvalue" );
  util_setenv( "ILLEGAL", "$illegal"); /* because value contains $ */
  

  errPushHandler( trap );

  if ( setjmp( env ) ) {
    i++;			/*  */
  }
  while (  strcmp( paths[i], "END" ) ) {
    
    printf( "%-25s -> %s\n", paths[i], util_logical_expand( paths[i] ) );
    i += 1;
  }

  i = 0;
  while (  strcmp( paths[i], "ERRORS" ) ) {
      char* exp =  util_logical_expand( paths[i] );
      char* comp=  util_logical_compress( exp, "OCTTOOLS" );
      printf( "%-25s ->>> %s\n", paths[i], comp );
      if ( strcmp( comp, paths[i] ) ) {
	  printf( "ERROR!!^^^^^^\n" );
      }
      i += 1;
  }


}

#endif


