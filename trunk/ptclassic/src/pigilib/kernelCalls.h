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

#include "mkIcon.h"

extern "C" {
	void clr_accum_string();
	void accum_string (const char*);
	void pr_accum_string();
}

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
        InterpUniverse (const char* name) : Runnable(KnownBlock::newSched(),
                                     KnownBlock::domain(),this)
        { setBlock(name,NULL);}
        void newSched() {
                delete scheduler;
                scheduler = KnownBlock::newSched();
                type = KnownBlock::domain();
        }

	Scheduler* mySched() { return scheduler;}
};
