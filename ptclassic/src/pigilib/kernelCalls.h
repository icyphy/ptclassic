/* kernelCalls.h  edg
Version identification:
$Id$
*/

#include "InterpGalaxy.h"
#include "Universe.h"

/* boolean data type for pigi */
typedef int boolean;

/* error handling function for pigi */
extern "C" void ErrAdd(const char*);

#include "mkIcon.h"
#include "paramStructs.h"

extern "C" {
	void clr_accum_string();
	void accum_string (const char*);
	void pr_accum_string();
	void FindClear();
}

