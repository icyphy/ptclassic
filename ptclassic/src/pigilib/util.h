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
extern char* setCurDomainS();
extern char* setCurDomainF();
extern char* setCurDomainInst();
