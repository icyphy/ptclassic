/**
    topBuf.c :: TOP Library

    Attempts to provide a safer version of vsprintf.  Uses a ring
    of (large) dynamically allocated buffers.  While mainly intended
    for string/vsprintf use, it can be used for other buffers as well.

    Note that this buffer might be used by the "topMsg" error reporting
    system, and thus to prevent recursion, we cant call any topMsg funcs.
    Thus all errors are fatal and are reported using write() to stderr
    followed by a core dump.

    Author: Kennard White

    Copyright (c) 1990-93 The Regents of the University of California.
    All rights reserved.  See the file "./copyright.h" for full copyright
    notice, limitation of liability, and disclaimer of warranty provisions.
#include "copyright.h"				COPYRIGHTENDKEY
**/
#if !defined(lint) && !defined(SABER)
static char SccsId[] = "$Id$";
#endif

#include "topFixup.h"
#include <stdarg.h>

#include "topStd.h"
#include "topMem.h"
#include "topBuf.h"

#define TOPBUF_MINBUFS 8

#define TOPBUF_TAG_LOCKED	((char)241)
#define TOPBUF_TAG_UNLOCKED	((char)242)

static int	_TopBufNumBufs = 0;
static char**	_TopBufBufs = NULL;
static int	_TopBufRotor;
static int	_TopBufNumLockedBufs;

static void
_topBufOverflow( char *buf) {
    char		*s;

    s = "\n\nFatal Error: Buffer overflow/corruption in topBuf.\n"; 
      write( 2, s, strlen(s));
    s = "Probable memory corruption...will abort program.\n";
      write( 2, s, strlen(s));
    if ( buf ) {
        s = "Contents of format buffer is below. Probable memory corruption.\n";
          write( 2, s, strlen(s));
          write( 2, buf, strlen(buf));
    }
    s = "...Aborting...\n";
      write( 2, s, strlen(s));

    abort();
    exit(1);
}

static int
_topBufGrowRing() {
    int		i, n, r;

    if ( _TopBufBufs == NULL || _TopBufNumBufs <= 0 ) {
	_TopBufNumBufs = 0;
	n = TOPBUF_MINBUFS;
    	_TopBufBufs = (char**) calloc( (unsigned)n, sizeof(char*));
    } else {
	n = _TopBufNumBufs*2;
        _TopBufBufs = (char**) realloc( _TopBufBufs, sizeof(char*)*n);
    }
    if ( _TopBufBufs == NULL ) {
        memMallocFail( (unsigned)n*sizeof(char*), "_topBufGrowRing()");
    }
    for ( i=_TopBufNumBufs; i < n; i++) {
	_TopBufBufs[i] = NULL;
    }
    r = _TopBufNumBufs;
    _TopBufNumBufs = n;
    return r;
}

void
topBufInit() {
    if ( _TopBufNumBufs > 0 )
	return;
    _TopBufRotor = 0;
    _TopBufNumLockedBufs = 0;
    _topBufGrowRing();
}

#define TOPBUF_CHECK_INITED() {			\
    if ( _TopBufNumBufs <= 0 )	topBufInit();	\
}

void
topBufUninit() {
    int		i;

    if ( _TopBufNumBufs == 0 )
	return;
    for ( i=0; i < _TopBufNumBufs; i++) {
	if ( _TopBufBufs[i] != NULL ) {
	    free( _TopBufBufs[i]);
	    _TopBufBufs[i] = NULL;
	}
    }
    _TopBufNumBufs = 0;
}

char*
topBufGet() {
    int		i, k;
    char	*buf;

    TOPBUF_CHECK_INITED();
    for (k=0; k < _TopBufNumBufs; k++) {
	if ( ++_TopBufRotor >= _TopBufNumBufs )	_TopBufRotor = 0;
	i = _TopBufRotor;
	if ( _TopBufBufs[i] == NULL ) {
	    /* allocate a buffer */
	    if ( (buf = (char*)malloc(TOP_MSG_BUFSIZE)) == NULL ) {
		memMallocFail( TOP_MSG_BUFSIZE, "topBufGet()");
		exit(1);
	    }
	    buf[0] = TOPBUF_TAG_UNLOCKED;
	    buf[1] = '\0';
	    _TopBufBufs[i] = buf;
	    return buf + 1;
	}
	if ( _TopBufBufs[i][0] == TOPBUF_TAG_UNLOCKED ) {
	    _TopBufBufs[i][1] = '\0';
	    return _TopBufBufs[i] + 1;
	}
    }

    _topBufGrowRing();
    return topBufGet();
}

char*
topBufLGet() {
    char	*buf;

    TOPBUF_CHECK_INITED();
    ++_TopBufNumLockedBufs;
    if ( (_TopBufNumBufs-_TopBufNumLockedBufs) < TOPBUF_MINBUFS ) {
    	_topBufGrowRing();
    }
    buf = topBufGet();
    *(buf-1) = TOPBUF_TAG_LOCKED;
    return buf;
}

void
topBufUnlock( char *buf) {
    if ( *(buf-1) != TOPBUF_TAG_LOCKED ) {
	_topBufOverflow(NULL);
    }
    --_TopBufNumLockedBufs;
    *(buf-1) = TOPBUF_TAG_UNLOCKED;
}

char*
_topBufStrcpy( char *buf, int buflen, char *str) {
    int			n;

    n = strlen( str);
    if ( n >= buflen - TOP_MSG_BUFPADSIZE ) {
	buf[buflen-1] = NULL;
	_topBufOverflow( buf);	/* aborts */
    }
    strcpy( buf, str);
    return buf;
}


char*
topBufStrcpy( char *str) {
    return _topBufStrcpy( topBufGet(), TOP_MSG_BUFSIZE, str);
}

char*
topBufLStrcpy( char *str) {
    return _topBufStrcpy( topBufLGet(), TOP_MSG_BUFSIZE, str);
}

char*
_topBufStrcat( char *buf, int buflen, char *str) {
    int			n, nb;

    nb = strlen(buf);
    n = strlen( str);
    if ( n+nb >= buflen - TOP_MSG_BUFPADSIZE ) {
	buf[buflen-1] = NULL;
	_topBufOverflow( buf);	/* aborts */
    }
    strcpy( buf+nb, str);
    return buf;
}

char*
topBufStrcat( char *buf, char *str) {
    return _topBufStrcat( buf, TOP_MSG_BUFSIZE, str);
}

char*
_topBufFmtVa( char *buf, int buflen, char *fmt, va_list args) {
    int			n;

    buf[0] = '\0';
    vsprintf( buf, fmt, args);
    n = strlen( buf);
    if ( n >= buflen - TOP_MSG_BUFPADSIZE ) {
	buf[buflen-1] = NULL;
	_topBufOverflow( buf);	/* aborts */
    }
    return buf;
}

char*
_topBufFmt( char *buf, int buflen, char *fmt, ...) {
    va_list		args;

    va_start( args, fmt);
    buf = _topBufFmtVa( buf, buflen, fmt, args);
    va_end( args);
    return buf;
}


char*
topBufFmtVa( char *fmt, va_list args) {
    return _topBufFmtVa( topBufGet(), TOP_MSG_BUFSIZE, fmt, args);
}

char*
topBufFmt( char *fmt, ...) {
    va_list		args;
    char*		buf;

    va_start( args, fmt);
    buf = topBufFmtVa( fmt, args);
    va_end( args);
    return buf;
}

char*
topBufLFmtVa( char *fmt, va_list args) {
    return _topBufFmtVa( topBufLGet(), TOP_MSG_BUFSIZE, fmt, args);
}

char*
topBufLFmt( char *fmt, ...) {
    va_list		args;
    char		*buf;

    va_start( args, fmt);
    buf = topBufLFmtVa( fmt, args);
    va_end( args);
    return buf;
}


char*
topBufAppendFmtVa( char *buf, char *fmt, va_list args) {
    int		len = strlen(buf);
    _topBufFmtVa( buf+len, TOP_MSG_BUFSIZE-len, fmt, args);
    return buf;
}

char*
topBufAppendFmt( char *buf, char *fmt, ...) {
    va_list		args;

    va_start( args, fmt);
    buf = topBufAppendFmtVa( buf, fmt, args);
    va_end( args);
    return buf;
}
