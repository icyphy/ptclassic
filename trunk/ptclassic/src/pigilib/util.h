/* 
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY
*/
/* util.h
Version identification:
$Id$
Header file for util.c
*/

#define UToolName "ptolemy"
    /* name of the tool that this RPC application is for */
#define UTechProp "ptolemy"  /* name of technology in technology directory */
#define UAppName "pigi"  /* name of this application */
#define UToolEnvName "PTOLEMY"	/* environment var name for tool */

#define UFree(buf) (void) free((buf))

extern char DEFAULT_DOMAIN[];

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

typedef struct DupSheetNode_s {
    char *info;
    struct DupSheetNode_s *next;
} DupSheetNode;
typedef DupSheetNode *DupSheet;
extern boolean DupSheetInit();
extern boolean DupSheetClear();
extern boolean DupSheetAdd();
extern boolean DupSheetIsDup();
extern char* getDomainF();
extern char* getDomainS();
extern char* getDomainInst();
extern char* setCurDomainS();
extern char* setCurDomainF();
extern char* setCurDomainInst();
