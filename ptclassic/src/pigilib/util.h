#ifndef _UTIL_H
#define _UTIL_H 1

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

/* util.h
Version identification:
@(#)util.h	1.20    02/05/97
Header file for util.c
*/

/* Do the right thing for sol2 boolean defs.  compat.h must be included
 * first so sys/types.h is included correctly.
 */
#include "sol2compat.h"

/* name of the tool that this RPC application is for */
#define UToolName "ptolemy"

/* name of technology in technology directory */
#define UTechProp "ptolemy"

/* name of this application */
#define UAppName "pigi"

/* environment var name for tool */
#define UToolEnvName "PTOLEMY"

#define UFree(buf) ((void) free((buf)))

extern char DEFAULT_DOMAIN[];

/* Define data structures lsList, octObject, and RPCSpot, respectively */
#include "list.h"
#include "oct.h"
#include "rpc.h"

typedef struct DupSheetNode_s {
    const char *info;
    const char *moreinfo;
    struct DupSheetNode_s *next;
} DupSheetNode;
typedef DupSheetNode *DupSheet;

extern boolean UMalloc ARGS((char **outPtr, int bytes));
extern boolean StrDup ARGS((char **outPtr, char *in));
extern char* DupString ARGS((const char *string));
extern char* SkipWhite ARGS((char *s));
extern char* BaseName ARGS((char *name));
extern char* DirName ARGS((char *buf));
extern boolean ConcatDirName ARGS((char *dir,char *baseName, char **outPtr));
extern boolean UGetFullTechDir ARGS((char **strPtr));
extern boolean YesNoToBoolean ARGS((char *str));
extern char* BooleanToYesNo ARGS((boolean bit));
extern boolean UniqNameInit();
extern char* UniqNameGet ARGS((char *s));

extern void DupSheetInit ARGS((DupSheet *ds));
extern void DupSheetClear ARGS((DupSheet *ds));
extern boolean DupSheetAdd ARGS((DupSheet *ds, const char *item,
				 const char *item2));
extern boolean DupSheetIsDup ARGS((DupSheet *ds, const char *item,
				   const char* item2));
extern const char* getDomainS ARGS((RPCSpot *spot));
extern const char* setCurDomainS ARGS((RPCSpot *spot));
extern const char* getDomainF ARGS((octObject *facetPtr));
extern const char* setCurDomainF ARGS((octObject *facetPtr));
extern const char* getDomainInst ARGS((octObject *instPtr));
extern const char* setCurDomainInst ARGS((octObject *instPtr));

#endif  /* _UTIL_H */
