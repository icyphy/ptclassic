/* utility.h  edg
Version identification:
$Id$
Header file for utility.c
*/

#define UToolName "ptolemy"
    /* name of the tool that this RPC application is for */
#define UTechProp "ptolemy"  /* name of technology in technology directory */
#define UAppName "pigi"  /* name of this application */

#define UFree(buf) (void) free((buf))

extern boolean UMalloc();
extern boolean StrDup();
extern char *DupString();
extern void TildeExpand();
extern char *SkipWhite();
extern char *BaseName();
extern boolean ConcatDirName();
extern boolean UGetTechDir();
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
