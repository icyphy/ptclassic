/* vemInterface.h  edg
Version identification:
$Id$
*/

extern void PrintCon();
extern void PrintConLog();
extern void PrintErr();
extern void PrintDebug();
extern void PrintDebugSet();
extern boolean PrintDebugGet();
extern void ViInit();
extern void ViTerm();
extern char *ViGetName();
extern boolean MyCopyFacet();

#define ViDone() \
    ViTerm(); \
    vemPrompt(); \
    return (RPC_OK);
