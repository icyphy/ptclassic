static const char file_id[] = "SDFMyRamp.pl";
// .cc file generated from SDFMyRamp.pl by ptlang
/*
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "SDFMyRamp.h"

const char *star_nm_SDFMyRamp = "SDFMyRamp";

const char* SDFMyRamp :: className() const {return star_nm_SDFMyRamp;}

ISA_FUNC(SDFMyRamp,SDFStar);

Block* SDFMyRamp :: makeNew() const { LOG_NEW; return new SDFMyRamp;}

SDFMyRamp::SDFMyRamp ()
{
	setDescriptor("Generate a ramp signal, starting at \"value\" (default 0) and\nincrementing by step size \"step\" (default 1) on each firing.");
	addPort(output.setPort("output",this,FLOAT));
	addState(step.setState("step",this,"1.0","Increment from one sample to the next."));
	addState(value.setState("value",this,"0.0","Initial (or latest) value output by Ramp.",
# line 32 "SDFMyRamp.pl"
A_SETTABLE|A_NONCONSTANT));


}

void SDFMyRamp::go() {
# line 35 "SDFMyRamp.pl"
double t = value;
		output%0 << t;
		t += step;
		value = t;
}

// prototype instance for known block list
static SDFMyRamp proto;
static RegisterBlock registerBlock(proto,"MyRamp");
