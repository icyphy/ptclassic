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
#include "iv.h"
#include "ivBuf.h"
#include <stdio.h>

static Display* ivBufDisplay;

#define IVRINGBELL( d ) { XBell ((d),50); XFlush( (d) ); }


void ivBufInit( buf, size, disp )
    ivBuf *buf;
    int size;
    Display* disp;
{
    buf->buf = (char*)malloc( (unsigned)size );
    if ( buf->buf != 0 ) {
	buf->size = size;
    } else {
	fprintf( stderr, "ivBuf: failed initialization: out of memory" );
    }
    ivBufDisplay = disp;
}

void ivBufClear( buf )
    ivBuf* buf;
{
    buf->count = 0;
    buf->buf[ 0 ] = '\0';
}

char* ivBufGetString( buf )
    ivBuf* buf;
{
    return buf->buf;
}

int ivBufGetLength( buf )
    ivBuf* buf;
{
    return buf->count;
}


void ivBufAddChar(buf, c) 
ivBuf* buf;
char c;
{
    if (  buf->count + 2 <= buf->size  ) {
	buf->buf[ buf->count++ ] = c;
	buf->buf[ buf->count ] = '\0';
    } else {

	IVRINGBELL( ivBufDisplay );
    }
    buf->buf[ buf->count ] = '\0';
}

void ivBufAddString( buf, s )
    ivBuf* buf;
    char* s;
{
    int len = strlen( s );

    if ( buf->count + len < buf->size ) {
	strcpy( &buf->buf[ buf->count ] , s ) ;
	buf->count += len ;
    } else {
	IVRINGBELL( ivBufDisplay );
    }
}

void ivBufDelChar( buf )
    ivBuf* buf;
{
    if ( buf->count > 0 ) { 
	buf->count--;
    } else {
	IVRINGBELL( ivBufDisplay );
    }
    buf->buf[ buf->count ] = '\0';

}

void ivBufDelWord( buf )
    ivBuf* buf;
{
    /*
     * Erase the count word on the line.
     */
    do {
	ivBufDelChar( buf );
    } while ( buf->count > 0 && buf->buf[ buf->count - 1 ] != ' ' ); 
}
