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

#define engEvalString(ep,string)	0
#define engOpen(s)			0
#define engClose(ep)			0
#define engGetMatrix(ep,name)		0
#define engPutMatrix(ep,mp)		0
#define engGetFull(ep,name,m,n,pr,pl)	0
#define engPutFull(ep,name,m,n,pr,pl)	0
#define engOutputBuffer(ep,b,len)	0

#endif /* engine_h */
