/* kernelCalls.h  edg
Version identification:
$Id$
*/

#include "InterpGalaxy.h"
#include "Universe.h"

/* boolean data type for pigi */
typedef int boolean;

/* error handling function for pigi */
extern "C" void ErrAdd(char*);

// Terminal data structs pigi
#define TERM_ARR_MAX 14  /* max # I/O terms = max # positions */
struct Term {
    char *name;
    boolean multiple;
};

struct TermList {
    Term in[TERM_ARR_MAX];
    int in_n;
    Term out[TERM_ARR_MAX];
    int out_n;
};

// Parameter structs for pigi
struct ParamStruct {
    const char *name;
    const char *value;
};
typedef struct ParamStruct ParamType;

struct ParamListStruct {
    int length; /* length of array */
    ParamType *array; /* points to first element */
};
typedef struct ParamListStruct ParamListType;

// An InterpUniverse is an Universe with an interpreted galaxy
// in it.  This is accomplished by making it Runnable and an InterpGalaxy.

class InterpUniverse : public InterpGalaxy, public Runnable {
public:
        InterpUniverse () : Runnable(KnownBlock::newSched(),
                                     KnownBlock::domain(),this)
        { setBlock("mainGalaxy",NULL);}
        void newSched() {
                delete scheduler;
                scheduler = KnownBlock::newSched();
                type = KnownBlock::domain();
        }

	Scheduler* mySched() { return scheduler;}
};
