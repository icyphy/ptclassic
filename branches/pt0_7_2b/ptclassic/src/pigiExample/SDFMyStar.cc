static const char file_id[] = "SDFMyStar.pl";
// .cc file generated from SDFMyStar.pl by ptlang
/*
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "SDFMyStar.h"
#include <math.h>

const char *star_nm_SDFMyStar = "SDFMyStar";

const char* SDFMyStar :: className() const {return star_nm_SDFMyStar;}

ISA_FUNC(SDFMyStar,SDFStar);

Block* SDFMyStar :: makeNew() const { LOG_NEW; return new SDFMyStar;}

SDFMyStar::SDFMyStar ()
{
	setDescriptor("This star computes the sine of its input (assumed to be an angle in radians) plus 1.0.");
	addPort(input.setPort("input",this,FLOAT));
	addPort(output.setPort("output",this,FLOAT));


}

void SDFMyStar::go() {
# line 24 "SDFMyStar.pl"
output%0 << sin (double(input%0)) + 1.0;
}

// prototype instance for known block list
static SDFMyStar proto;
static RegisterBlock registerBlock(proto,"MyStar");
