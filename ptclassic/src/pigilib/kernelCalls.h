#ifndef KERNELCALLS_H
#define KERNELCALLS_H 1
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
/* kernelCalls.h  edg
Version identification:
$Id$
*/

/* Do the right thing for sol2 boolean defs.  compat.h must be included
 * first so sys/types.h is included correctly.  See octtools/include/port.h
 */
#include "sol2compat.h"

#ifdef __cplusplus

#include "InterpGalaxy.h"
#include "Universe.h"

#ifndef PARAMSTRUCTS_H
#include "paramStructs.h"
#endif
#ifndef MKICON_H
#include "mkIcon.h"
#endif

extern "C" {
extern boolean KcInitLog(const char* file);
extern void KcLog(const char* str);
extern void KcFlushLog();
extern void testParse(const char* name);
extern const char* callParseClass (char* name);
extern const char * KcDomainOf(char* name);
extern void KcClearUniverse(const char* name);
extern boolean KcInstance(char *name, char *ako, ParamListType* pListPtr);
extern boolean KcMakeState(char *name, char *type, char *initVal);
extern boolean KcConnect(char *inst1, char *t1, char *inst2, char *t2, char* delay, char* width);
extern boolean KcAlias(char *fterm, char *inst, char *aterm);
extern boolean KcSetKBDomain(const char* domain);
extern const char*curDomainName();
extern boolean KcDefgalaxy(const char *galname, const char *domain, const char* innerTarget);
extern boolean KcEndDefgalaxy(const char* outerDomain);
extern void KcSetDesc(const char* desc);
extern boolean KcDisplaySchedule();
extern void KcEditSeed(int n);
extern boolean KcIsKnown(char *className);
extern boolean KcIsCompiledInStar(char *className);
extern boolean KcGetTerms(char* name, TermList* terms);
extern boolean KcIsMulti(char* blockname, char* portname);
extern boolean KcGetParams(char* name, ParamListType* pListPtr);
extern boolean KcGetTargetParams(char* name, ParamListType* pListPtr);
extern boolean KcModTargetParams(ParamListType* pListPtr);
extern boolean KcInfo(char* name, char** info);
extern int KcProfile (char* name);
extern boolean KcNumPorts (char* starname, char* portname, int numP);
extern int numberOfDomains();
extern const char* nthDomainName(int n);
extern int KcNode (const char* name);
extern int KcNodeConnect(const char* inst, const char* term, const char* node);
extern int KcDomainTargets(const char* domain, const char** names, int nMax);
extern int KcSetTarget(const char* targetName);
extern const char* KcDefTarget(const char* domain);
extern void KcSetAnimationState(int s);
extern int KcGetAnimationState();
extern const char* KcExpandPathName(const char* name);
extern void KcCatchSignals();


/* from pigiLoader.h */
extern void KcLoadInit (const char* argv0);
extern void KcDoStartupLinking();
extern int FindStarSourceFile(const char* dir,const char* dom,const char* base, char* buf);
extern int KcCompileAndLink (const char* name, const char* idomain, const char* srcDir, int permB, const char* linkArgs);
extern int KcLoad (const char* iconName, int permB, const char* linkArgs);
/* error handling function for pigi */
extern void ErrAdd(const char*);
extern	void clr_accum_string();
extern	void accum_string (const char*);
extern	void pr_accum_string();
extern	void FindClear();
}
#else /* __cplusplus */
/* Functions defined in kernelCalls.h */
extern boolean KcInstance(/*char *name, char *ako, ParamListType* pListPtr*/);
extern boolean KcMakeState(/*char *name, char *type, char *initVal*/);
extern boolean KcConnect(/*char *inst1, char *t1, char *inst2, char *t2, char* delay, char* width*/);
extern boolean KcAlias(/*char *fterm, char *inst, char *aterm*/);
extern boolean KcSetKBDomain(/*const char* domain*/);
extern boolean KcDefgalaxy(/*const char *galname, const char *domain, const char* innerTarget*/);
extern boolean KcEndDefgalaxy(/*const char* outerDomain*/);
extern boolean KcDisplaySchedule();
extern boolean KcIsKnown(/* char *className */);
extern boolean KcIsCompiledInStar(/* char *className */);
extern boolean KcGetTerms(/* char* name, TermList* terms */);
extern boolean KcModTargetParams(/* ParamListType* pListPtr */);
extern boolean KcIsMulti(/* char* blockname, char* portname */);
extern boolean KcGetParams(/* char* name, ParamListType* pListPtr */);
extern boolean KcGetTargetParams(/* char* name,
				    ParamListType* pListPtr */);
extern int KcProfile(/* char* name */);
extern int numberOfDomains();
extern int KcNodeConnect(/* const char* inst, const char* term, const
			    char* node */); 
extern int KcNode (/* const char* name */);

extern int KcEventLoopActive();
extern void KcSetEventLoop(/* int on */);

/* error handling function for pigi */
extern void ErrAdd(/* const char* */);
extern	void clr_accum_string();
extern	void accum_string (/* const char* */);
extern	void pr_accum_string();
extern	void FindClear();

#endif /* __cplusplus */

#endif /* KERNELCALLS_H */
