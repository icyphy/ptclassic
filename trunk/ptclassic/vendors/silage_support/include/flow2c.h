/* $Id$ */

#include "dag.h"
#define UNVISITED 0
#define VISITED 1

#define NEW(type) ( (type *) tmalloc(sizeof(type)))

typedef enum {false, true} bool;

typedef struct ArrayDefinition *ArrayDefinitionList;
typedef struct NameListDefinition *NameListDefinitionList;

/* for storing all fixedtype arrays declarations in a function.  We have
   to make 1st pass to handle tmp[0], tmp[1], tmp[2] as input edges      */
typedef struct ArrayDefinition {
    EdgePointer edge;
    char *Name;
    int index[4];
    int fix;
    ArrayDefinitionList Next;
} ArrayDefinition;

/* for storing all loop indices declarations in a function */
typedef struct NameListDefinition {
    char *Name;
    NameListDefinitionList Next;
} NameListDefinition;

/* macro's */

#define GE(a) ((eGraphPointer) a->Extension)
#define NE(a) ((eNodePointer) a->Extension)
#define EE(a) ((eEdgePointer) a->Extension)
#define GetProc(n) (NE(n)->proc)
#define GetStage(n) (NE(n)->stage)
#define GetStep(n) (NE(n)->step)

typedef struct eGraph {
    int visit;
    int stage;
    bool HasDelay;
    ListPointer ListOfFuncApps;
    ListPointer ListOfDelays;
    ListPointer ListOfLoopDelays;
    int NumberOfProcesses;
} eGraph, *eGraphPointer;

typedef struct eNode {
    int proc;
    int stage;
    int step;
    int visit;
} eNode, *eNodePointer;

typedef struct eEdge {
    int visit;
    GraphPointer graph;
} eEdge, *eEdgePointer;
