static const char file_id[] = "SRCGCAdd.pl";
// .cc file generated from SRCGCAdd.pl by ptlang
/*
Copyright (c) 1998 Supélec & Thomson-CSF Optronique.
    All rights reserved.
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "SRCGCAdd.h"

const char *star_nm_SRCGCAdd = "SRCGCAdd";

const char* SRCGCAdd :: className() const {return star_nm_SRCGCAdd;}

ISA_FUNC(SRCGCAdd,SRCGCStar);

Block* SRCGCAdd :: makeNew() const { LOG_NEW; return new SRCGCAdd;}

CodeBlock SRCGCAdd :: code (
# line 75 "SRCGCAdd.pl"
"    if( SRCGC_ISPRESENT($ref(input1)) && SRCGC_ISPRESENT($ref(input2)) ) {\n"
"      SRCGC_EMIT($ref(output),SRCGC_GET($ref(input1)) + SRCGC_GET($ref(input2)));\n"
"    } else if (SRCGC_ISPRESENT($ref(input1)) && SRCGC_ISABSENT($ref(input2)) ) {\n"
"      SRCGC_EMIT($ref(output),SRCGC_GET($ref(input1)));\n"
"    } else if (SRCGC_ISABSENT($ref(input1)) && SRCGC_ISPRESENT($ref(input2)) ) {\n"
"      SRCGC_EMIT($ref(output),SRCGC_GET($ref(input2)));\n"
"    }\n");

SRCGCAdd::SRCGCAdd ()
{
	setDescriptor("Add the two inputs.");
	addPort(input1.setPort("input1",this,INT));
	addPort(input2.setPort("input2",this,INT));
	addPort(output.setPort("output",this,INT));

# line 73 "SRCGCAdd.pl"
noInternalState();
}

int SRCGCAdd::myExecTime() {
# line 88 "SRCGCAdd.pl"
return 0;
}

void SRCGCAdd::setup() {
# line 70 "SRCGCAdd.pl"
reactive();
}

void SRCGCAdd::go() {
# line 85 "SRCGCAdd.pl"
addCode(code);
}

// prototype instance for known block list
static SRCGCAdd proto;
static RegisterBlock registerBlock(proto,"Add");
