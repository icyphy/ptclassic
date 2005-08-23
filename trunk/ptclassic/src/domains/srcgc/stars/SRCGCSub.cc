static const char file_id[] = "SRCGCSub.pl";
// .cc file generated from SRCGCSub.pl by ptlang
/*
Copyright (c) 1998 Supélec & Thomson-CSF Optronique.
    All rights reserved.
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "SRCGCSub.h"

const char *star_nm_SRCGCSub = "SRCGCSub";

const char* SRCGCSub :: className() const {return star_nm_SRCGCSub;}

ISA_FUNC(SRCGCSub,SRCGCStar);

Block* SRCGCSub :: makeNew() const { LOG_NEW; return new SRCGCSub;}

CodeBlock SRCGCSub :: code (
# line 75 "SRCGCSub.pl"
"    if (SRCGC_ISPRESENT($ref(input1)) && SRCGC_ISPRESENT($ref(input2)) ) {\n"
"      SRCGC_EMIT($ref(output),SRCGC_GET($ref(input1)) - SRCGC_GET($ref(input2)));\n"
"    } else if (SRCGC_ISPRESENT($ref(input1)) && SRCGC_ISABSENT($ref(input2)) ) {\n"
"      SRCGC_EMIT($ref(output),SRCGC_GET($ref(input1)));\n"
"    } else if (SRCGC_ISABSENT($ref(input1)) && SRCGC_ISPRESENT($ref(input2)) ) {\n"
"      SRCGC_EMIT($ref(output),SRCGC_GET($ref(input2)));\n"
"    }\n");

SRCGCSub::SRCGCSub ()
{
	setDescriptor("Sub the two inputs.");
	addPort(input1.setPort("input1",this,INT));
	addPort(input2.setPort("input2",this,INT));
	addPort(output.setPort("output",this,INT));

# line 73 "SRCGCSub.pl"
noInternalState();
}

int SRCGCSub::myExecTime() {
# line 88 "SRCGCSub.pl"
return 0;
}

void SRCGCSub::setup() {
# line 70 "SRCGCSub.pl"
reactive();
}

void SRCGCSub::go() {
# line 85 "SRCGCSub.pl"
addCode(code);
}

// prototype instance for known block list
static SRCGCSub proto;
static RegisterBlock registerBlock(proto,"Sub");
