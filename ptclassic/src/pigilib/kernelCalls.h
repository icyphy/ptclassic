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
