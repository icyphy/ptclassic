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
 * Provide a dummy C include header file that mimics "engine.h" that
 * comes with Matlab.
 *
 */

#ifndef engine_h
#define engine_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "matrix.h"

typedef char Engine;

extern int engEvalString ARGS((Engine* enginePtr, char* command));
extern Engine* engOpen ARGS((char* unixCommand));
extern int engClose ARGS((Engine* enginePtr));
extern Matrix* engGetMatrix ARGS((Engine* enginePtr, char* name));
extern int engPutMatrix ARGS((Engine* enginePtr, Matrix* matrixPtr));
extern int engGetFull ARGS((Engine* enginePtr, char* name,
			    int* numrows, int* numcols,
			    Real** realPartPtr, Real** imagePartPtr));
extern int engPutFull ARGS((Engine* enginePtr, char* name,
			    int numrows, int numcols,
			    Real* realPartPtr, Real* imagePartPtr));
extern int engOutputBuffer ARGS((Engine* enginePtr, char* buffer,
				 int bufferlen));

#ifdef __cplusplus
}
#endif

#endif /* engine_h */
