#ifndef _kernelCalls_h
#define _kernelCalls_h 1

/* 
Copyright (c) 1990-1997 The Regents of the University of California.
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

/* kernelCalls.h  edg
Version identification:
@(#)kernelCalls.h	2.21	02/05/97

This is a C include file, and NOT a C++ include file.
*/

/* Do the right thing for sol2 boolean defs.  compat.h must be included
 * first so sys/types.h is included correctly.  See octtools/include/port.h
 */
#include "sol2compat.h"
#include "compat.h"

#include "paramStructs.h"

extern boolean KcInitLog ARGS((const char* file));
extern void KcLog ARGS((const char* str));
extern void KcFlushLog();
extern void testParse ARGS((const char* name));
extern const char* callParseClass ARGS((char* name));
extern const char* KcDomainOf ARGS((const char* name));
extern boolean KcInstance ARGS((char* name, char* ako, ParamListType* pListPtr));
extern boolean KcMakeState ARGS((char* name, char* type, char* initVal));
extern boolean KcConnect ARGS((char* inst1, char* t1, char* inst2, char* t2,
			 char* delay, char* width));
extern boolean KcAlias ARGS((char* fterm, char* inst, char* aterm));
extern boolean KcSetKBDomain ARGS((const char* domain));
extern const char* curDomainName();
extern boolean KcDefgalaxy ARGS((const char* galname, const char* domain,
			   const char* innerTarget));
extern boolean KcEndDefgalaxy ARGS((const char* definitionSource,
				    const char* outerDomain));
extern void KcSetDesc ARGS((const char* desc));
extern boolean KcDisplaySchedule();
extern void KcEditSeed ARGS((int n));
extern boolean KcIsKnown ARGS((const char* className));
extern const char* KcGalaxyDefSource ARGS((const char* baseName,
					   const char* domain));
extern const char* KcUniverseDefSource ARGS((const char* baseName));
extern void KcSetUniverseDefSource ARGS((const char* pathName));
extern boolean KcIsCompiledInStar ARGS((const char* className));
extern boolean KcCheckTerms ARGS((const char* name, const char** newNames,
				  const char** newTypes, int* newIsOut,
				  int* numOrdPortsPtr, int* newNameCountPtr));
extern boolean KcIsMulti ARGS((char* blockname, char* portname));
extern boolean KcGetParams ARGS((char* name, ParamListType* pListPtr));
extern boolean KcGetTargetParams ARGS((char* name, ParamListType* pListPtr));
extern boolean KcModTargetParams ARGS((ParamListType* pListPtr));
extern boolean KcInfo ARGS((const char* name, char** info));
extern int KcPrintTerms ARGS((const char* name));
extern int KcProfile ARGS((const char* name));
extern boolean KcNumPorts ARGS((char* starname, char* portname, int numP));
extern int numberOfDomains();
extern const char* nthDomainName ARGS((int n));
extern int KcNode ARGS((const char* name));
extern int KcNodeConnect ARGS((const char* inst, const char* term,
			       const char* node));
extern int KcDomainTargets ARGS((const char* domain, const char** names,
				 int nMax));
extern int KcSetTarget ARGS((const char* targetName));
extern const char* KcDefTarget ARGS((const char* domain));
extern void TildeExpand ARGS((const char *input, char* buffer));
extern void KcCatchSignals();
extern int KcEventLoopActive();
extern void KcSetEventLoop ARGS((int on));
extern const char* HashString ARGS((const char* arg));

#endif /* _kernelCalls_h */
