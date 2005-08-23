static const char file_id[] = "SRCGCIntToString.pl";
// .cc file generated from SRCGCIntToString.pl by ptlang
/*
Copyright (c) 1998 Supélec & Thomson-CSF Optronique.
    All rights reserved.
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "SRCGCIntToString.h"

const char *star_nm_SRCGCIntToString = "SRCGCIntToString";

const char* SRCGCIntToString :: className() const {return star_nm_SRCGCIntToString;}

ISA_FUNC(SRCGCIntToString,SRCGCStar);

Block* SRCGCIntToString :: makeNew() const { LOG_NEW; return new SRCGCIntToString;}

CodeBlock SRCGCIntToString :: code (
# line 73 "SRCGCIntToString.pl"
"    if (!SRCGC_ISKNOWN($ref(output))) {\n"
"      char theString[10];\n"
"      sprintf( theString, \"%d\", SRCGC_GET($ref(input)) );\n"
"      SRCGC_EMIT($ref(output),theString);\n"
"    }\n");

SRCGCIntToString::SRCGCIntToString ()
{
	setDescriptor("Convert an integer input to a string output");
	addPort(input.setPort("input",this,INT));
	addPort(output.setPort("output",this,STRING));


}

void SRCGCIntToString::setup() {
# line 70 "SRCGCIntToString.pl"
noInternalState();
    reactive();
}

void SRCGCIntToString::go() {
# line 82 "SRCGCIntToString.pl"
addCode(code);
}

// prototype instance for known block list
static SRCGCIntToString proto;
static RegisterBlock registerBlock(proto,"IntToString");
