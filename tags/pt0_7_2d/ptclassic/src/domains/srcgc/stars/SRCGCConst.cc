static const char file_id[] = "SRCGCConst.pl";
// .cc file generated from SRCGCConst.pl by ptlang
/*
Copyright (c) 1998 Supélec & Thomson-CSF Optronique.
    All rights reserved.
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "SRCGCConst.h"

const char *star_nm_SRCGCConst = "SRCGCConst";

const char* SRCGCConst :: className() const {return star_nm_SRCGCConst;}

ISA_FUNC(SRCGCConst,SRCGCStar);

Block* SRCGCConst :: makeNew() const { LOG_NEW; return new SRCGCConst;}

SRCGCConst::SRCGCConst ()
{
	setDescriptor("In each instant, output a signal with value given by the ``level'' parameter.");
	addPort(output.setPort("output",this,INT));
	addState(level.setState("level",this,"0","The constant value"));

# line 70 "SRCGCConst.pl"
noInternalState();
}

int SRCGCConst::myExecTime() {
# line 76 "SRCGCConst.pl"
return 0;
}

void SRCGCConst::go() {
# line 73 "SRCGCConst.pl"
addCode("\tSRCGC_EMIT($ref(output),$val(level));\n");
}

// prototype instance for known block list
static SRCGCConst proto;
static RegisterBlock registerBlock(proto,"Const");
