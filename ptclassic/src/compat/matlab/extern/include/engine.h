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

inline Engine *engOpenPtError()
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
