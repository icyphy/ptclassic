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
/* util.h
Version identification:
$Id$
Header file for util.c
*/

/* Do the right thing for sol2 boolean defs.  compat.h must be included
 * first so sys/types.h is included correctly.
 */
#include "sol2compat.h"

#define UToolName "ptolemy"
    /* name of the tool that this RPC application is for */
#define UTechProp "ptolemy"  /* name of technology in technology directory */
#define UAppName "pigi"  /* name of this application */
#define UToolEnvName "PTOLEMY"	/* environment var name for tool */

#define UFree(buf) (void) free((buf))

extern char DEFAULT_DOMAIN[];

#ifdef __cplusplus
extern boolean UMalloc(char **outPtr, int bytes);
extern boolean StrDup(char **outPtr, char *in);
extern char *DupString(char *string);
extern void TildeExpand(char *input, char *buffer);
extern char *SkipWhite(char *s);
extern char *BaseName(char *name);
extern boolean ConcatDirName(char *dir,char *baseName, char **outPtr);
extern boolean UGetFullTechDir(char **strPtr);
extern boolean YesNoToBoolean(char *str);
extern char *BooleanToYesNo(boolean bit);
extern boolean UniqNameInit();
extern char *UniqNameGet(char *s);
extern boolean IsBalancedParen(); /* FIXME: this funct not in util.c */
extern char *DirName(char *buf);
#else
extern boolean UMalloc();
extern boolean StrDup();
extern char *DupString();
extern void TildeExpand();
extern char *SkipWhite();
extern char *BaseName();
extern boolean ConcatDirName();
extern boolean UGetFullTechDir();
extern boolean YesNoToBoolean();
extern char *BooleanToYesNo();
extern boolean UniqNameInit();
extern char *UniqNameGet();
extern boolean IsBalancedParen();
extern char *DirName();
#endif /* __cplusplus*/

typedef struct DupSheetNode_s {
    char *info;
    char *moreinfo;
    struct DupSheetNode_s *next;
} DupSheetNode;
typedef DupSheetNode *DupSheet;

#ifdef __cplusplus
#include "rpc.h"     /* needed to define "RPCSpot" */

extern void DupSheetInit(DupSheet *ds);
extern void DupSheetClear(DupSheet *ds);
extern boolean DupSheetAdd(DupSheet *ds, char *item);
extern boolean DupSheetAdd2(DupSheet *ds, char *item, char *item2);
extern boolean DupSheetIsDup(DupSheet *ds, char *item);
extern boolean DupSheetIsDup2(DupSheet *ds, char *item, char* item2);
extern char* setCurDomainS(RPCSpot *spot);
extern char* setCurDomainF(octObject *facetPtr);
extern char* setCurDomainInst(octObject *instPtr);
#else
extern void DupSheetInit();
extern void DupSheetClear();
extern boolean DupSheetAdd();
extern boolean DupSheetAdd2(/* DupSheet *ds, char *item, char *item2 */);
extern boolean DupSheetIsDup();
extern boolean DupSheetIsDup(/* DupSheet *ds, char *item */);
extern boolean DupSheetIsDup2(/* DupSheet *ds, char *item, char* item2 */);
extern char* getDomainF();
extern char* getDomainS();
extern char* getDomainInst();
extern char* setCurDomainS();
extern char* setCurDomainF();
extern char* setCurDomainInst();
#endif /* __cplusplus */
