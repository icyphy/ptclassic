static const char file_id[] = "XXXNop.pl";
// .cc file generated from XXXNop.pl by ptlang
/*
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "XXXNop.h"

const char *star_nm_XXXNop = "XXXNop";

const char* XXXNop :: className() const {return star_nm_XXXNop;}

ISA_FUNC(XXXNop,XXXStar);

Block* XXXNop :: makeNew() const { LOG_NEW; return new XXXNop;}

XXXNop::XXXNop ()
{
	setDescriptor("Does nothing");
	addPort(output.setPort("output",this,ANYTYPE));
	output.inheritTypeFrom(input);
	addPort(input.setPort("input",this,ANYTYPE));


}

void XXXNop::go() {
# line 23 "XXXNop.pl"
}

// prototype instance for known block list
static XXXNop proto;
static RegisterBlock registerBlock(proto,"Nop");
