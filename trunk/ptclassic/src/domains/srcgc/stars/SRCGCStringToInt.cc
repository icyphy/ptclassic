static const char file_id[] = "SRCGCStringToInt.pl";
// .cc file generated from SRCGCStringToInt.pl by ptlang
/*
Copyright (c) 1998 Supélec & Thomson-CSF Optronique.
    All rights reserved.
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "SRCGCStringToInt.h"

const char *star_nm_SRCGCStringToInt = "SRCGCStringToInt";

const char* SRCGCStringToInt :: className() const {return star_nm_SRCGCStringToInt;}

ISA_FUNC(SRCGCStringToInt,SRCGCStar);

Block* SRCGCStringToInt :: makeNew() const { LOG_NEW; return new SRCGCStringToInt;}

CodeBlock SRCGCStringToInt :: code (
# line 73 "SRCGCStringToInt.pl"
"    if (!SRCGC_ISKNOWN($ref(output))) {\n"
"      if (SRCGC_ISABSENT($ref(input))) { \n"
"        SRCGC_MAKEABSENT($ref(output));\n"
"      } else {\n"
"        int val = atoi( SRCGC_GET($ref(input)));\n"
"        SRCGC_EMIT($ref(output),val);\n"
"      }\n"
"    }\n");

SRCGCStringToInt::SRCGCStringToInt ()
{
	setDescriptor("Convert a string input to an integer output");
	addPort(input.setPort("input",this,STRING));
	addPort(output.setPort("output",this,INT));


}

void SRCGCStringToInt::setup() {
# line 70 "SRCGCStringToInt.pl"
noInternalState();
    reactive();
}

void SRCGCStringToInt::go() {
# line 84 "SRCGCStringToInt.pl"
addCode(code);
}

// prototype instance for known block list
static SRCGCStringToInt proto;
static RegisterBlock registerBlock(proto,"StringToInt");
