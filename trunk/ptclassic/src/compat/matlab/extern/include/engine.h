/*
 * $Id$
 *
 * Provide a dummy header that mimics "engine.h" that comes with Matlab.
 *
 */

#ifndef engine_h
#define engine_h

#include <stdio.h>
#include "matrix.h"

typedef char Engine;

Engine *engOpenPtError()
{
	Error::abortRun( "The external interface to Matlab has not been ",
			 "compiled into Ptolemy." );
	return( (Engine *) 0 );
}

#define engEvalString(ep,string)	1
#define engOpen(s)			engOpenPtError()
#define engClose(ep)			1
#define engGetMatrix(ep,name)		0
#define engPutMatrix(ep,mp)		1
#define engGetFull(ep,name,m,n,pr,pl)	1
#define engPutFull(ep,name,m,n,pr,pl)	1
#define engOutputBuffer(ep,b,len)

#endif /* engine_h */
