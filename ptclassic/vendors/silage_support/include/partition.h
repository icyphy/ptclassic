/* $Id$ */

#define MAX(A,B) ((A) > (B) ? (A) : (B))
#define MIN(A,B) ((A) < (B) ? (A) : (B))
#define ABS(A)   ((A) >= 0 ? (A) : -(A))
#define ARRAY(NAME, DIM, I, J) (NAME[I*DIM+J])
#define MAXPROC 64
#define MAXINT 64
#define INFINITY 100000000.0
#define UNVISITED 0
#define VISITED 1
#define INDV 0
#define AVG 1
#define FAIL -1
#define SUCCEED 0

#define TRANSCOST 0.1   /* b - a */
#define BYPASS  0.1     /* delay s->start in bus i and s->start in bus i+1 */
                      /* should be at least TRANSCOST, the time for a write */
#define TRANSINTERVAL 0  /* At least 1 instruction between each s->start */

#define INPUTCOST 175.0
#define OUTPUTCOST 975.0

#define SINCOST 46.0
#define COSCOST 46.0
#define SQRTCOST 19.0

#define INTPLUSCOST 1.0
#define FIXPLUSCOST 9.25
#define INTMINUSCOST 1.0
#define FIXMINUSCOST 9.25
#define INTMULTCOST 4.0
#define FIXMULTCOST 10.0
#define INTDIVCOST 7.0
#define FIXDIVCOST 14.5
#define INCCOST 1.0
#define DECCOST 1.0
#define INTLSHCOST 1.0
#define FIXLSHCOST 11.0
#define INTRSHCOST 1.0
#define FIXRSHCOST 11.0
#define INTANDCOST 1.0
#define FIXANDCOST 10.0
#define INTORCOST 1.0
#define FIXORCOST 10.0
#define INTEXORCOST 1.0
#define FIXEXORCOST 10.0
#define INTNOTCOST 1.0
#define FIXNOTCOST 10.0
#define INTASSIGNCOST 1.0
#define FIXASSIGNCOST 1.0
#define INTEQCOST 1.0
#define FIXEQCOST 10.0
#define INTNTECOST 1.0
#define FIXNTECOST 10.0
#define INTGTCOST 1.0
#define FIXGTCOST 10.0
#define INTGTECOST 1.0
#define FIXGTECOST 10.0
#define INTLTCOST 1.0
#define FIXLTCOST 10.0
#define INTLTECOST 1.0
#define FIXLTECOST 10.0
#define READCOST 3.0
#define WRITECOST 4.0
#define MUXCOST 2.0
#define LOOPHEAD 1.0
#define LOOPTEST 2.0 
#define FUNCHEAD 0.5
#define FUNCPARAM 1.0
#define STARTUP 0.0
#define CONSTCOST 0.0
#define BITMERGECOST 4.0
#define BITSELECTCOST 4.0
#define EXITCOST 1.0
#define DELAYINIT 9.5
#define DELAYUPDATE 5.5
#define LPDELAYUPDATE 5.5
#define DELAYACCESS1 3.0
#define DELAYACCESS2 3.0
#define INTCASTCOST 1.0
#define FIXCASTCOST 1.0
#define INTINITCOST 10.0
#define FIXINITCOST 10.0
#define INTNEGCOST 1.0
#define FIXNEGCOST 9.25
#define UPSAMPLECOST 0.0
#define DOWNSAMPLECOST 0.0
#define INTERPOLATECOST 0.0
#define DECIMATECOST 0.0
#define TIMEMUXCOST 0.0
#define TIMEDEMUXCOST 0.0
#define NOPCOST 0.0

typedef enum {false, true} bool;

/* macro's */

#define GE(a) ((eGraphPointer) a->Extension)
#define NE(a) ((eNodePointer) a->Extension)
#define TIME(s,T,t) ((s*T)+t)
#define GetCost(n) (NE(n)->cost)
#define GetProcNum(n) (NE(n)->procnum)
#define GetOutLevel(n) (NE(n)->outlevel)
#define GetInLevel(n) (NE(n)->inlevel)
#define GetStageNum(n) (NE(n)->stagenum)
#define GetDoneTime(n) (NE(n)->donetime)

typedef struct _LOOP *LoopPointer;

typedef struct _LOOP {
    char *Name;
    ListPointer LoopNode;
    int delaylimit;
    int delayleft;
    float loopcost;
    LoopPointer Next;
} _LOOP;

typedef struct _VARNODE *VarNodePointer;

typedef struct _VARNODE {
    int increase;
    int begin;
    int end;
    int step;
} _VARNODE;

typedef struct eNode {
    int inputcnt;
    float cost;
    float inlevel;
    float outlevel;
    int stagenum;
    float donetime;
    float F[MAXPROC];
    int stage[MAXPROC];
    int procnum;
    int visit;
    int order;
    NodePointer Prev;
} eNode, *eNodePointer;

typedef struct eGraph {
    float cost;
    LoopPointer ListOfLoops;   
} eGraph, *eGraphPointer;

typedef struct PROCESSOR {
    float exu_time;
    float done_time;
    ListPointer NodeList;
} PROCESSOR;
