/*  ganttIfc.h  edg
Version identification:
$Id$
*/

extern void FrameSetFacet();
extern int CreateFrames();
extern int Frame();
extern int ClearFrames();
extern int DestroyFrames();
extern int GanttMan();
extern void GanttErr();

/* Functions below should only be called by GGI */
extern boolean GanttRun();
extern boolean GanttInit();
extern void FindClear();
extern int RpcFindName();

typedef struct RgbValue_s {
    unsigned short red;
    unsigned short green;
    unsigned short blue;
} RgbValue;
