static const char file_id[] = "SRCGCPre.pl";
// .cc file generated from SRCGCPre.pl by ptlang
/*
Copyright (c) 1998 Supélec & Thomson-CSF Optronique.
    All rights reserved.
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "SRCGCPre.h"
#include <stream.h>

const char *star_nm_SRCGCPre = "SRCGCPre";

const char* SRCGCPre :: className() const {return star_nm_SRCGCPre;}

ISA_FUNC(SRCGCPre,SRCGCNonStrictStar);

Block* SRCGCPre :: makeNew() const { LOG_NEW; return new SRCGCPre;}

SRCGCPre::SRCGCPre ()
{
	setDescriptor("Delay the input by an instant.  Absent inputs are ignored.\n    Output is always present.");
	addPort(input.setPort("input",this,INT));
	addPort(output.setPort("output",this,INT));
	addState(theState.setState("theState",this,"0","Initial output value, state afterwards."));


}

void SRCGCPre::setup() {
# line 82 "SRCGCPre.pl"
input.independent();
}

void SRCGCPre::go() {
# line 86 "SRCGCPre.pl"
addCode("\tif (!SRCGC_ISKNOWN($ref(output))) {\n"
            "\tSRCGC_EMIT($ref(output),$ref(theState));\n\t}\n");
}

void SRCGCPre::tick() {
# line 91 "SRCGCPre.pl"
addCode("\tif ( SRCGC_ISPRESENT($ref(input))) {\n"
            "\t $ref(theState) = SRCGC_GET($ref(input));\n\t}\n");
}

// prototype instance for known block list
static SRCGCPre proto;
static RegisterBlock registerBlock(proto,"Pre");
