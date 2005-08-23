static const char file_id[] = "SRCGCAnd.pl";
// .cc file generated from SRCGCAnd.pl by ptlang
/*
Copyright (c) 1998 Supélec & Thomson-CSF Optronique.
    All rights reserved.
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "SRCGCAnd.h"

const char *star_nm_SRCGCAnd = "SRCGCAnd";

const char* SRCGCAnd :: className() const {return star_nm_SRCGCAnd;}

ISA_FUNC(SRCGCAnd,SRCGCNonStrictStar);

Block* SRCGCAnd :: makeNew() const { LOG_NEW; return new SRCGCAnd;}

CodeBlock SRCGCAnd :: code (
# line 79 "SRCGCAnd.pl"
"    if ( !SRCGC_ISKNOWN($ref(output)) ) {\n"
"      if (  SRCGC_ISPRESENT($ref(input1)) && !SRCGC_GET($ref(input1))\n"
"          ||SRCGC_ISPRESENT($ref(input2)) && !SRCGC_GET($ref(input2)) ) {\n"
"        SRCGC_EMIT($ref(output),0);\n"
"      } else if (  SRCGC_ISPRESENT($ref(input1)) && SRCGC_GET($ref(input2))\n"
"                 &&SRCGC_ISPRESENT($ref(input2)) && SRCGC_GET($ref(input2)) ) {\n"
"        SRCGC_EMIT($ref(output),1);\n"
"      }\n"
"    }\n");

SRCGCAnd::SRCGCAnd ()
{
	setDescriptor("Lazy logical AND: output is true if both inputs are present and true,\n    output is false if one is present and false, output is absent if both\n    are absent.");
	addPort(input1.setPort("input1",this,INT));
	addPort(input2.setPort("input2",this,INT));
	addPort(output.setPort("output",this,INT));

# line 76 "SRCGCAnd.pl"
noInternalState();
    reactive();
}

int SRCGCAnd::myExecTime() {
# line 95 "SRCGCAnd.pl"
return 0;
}

void SRCGCAnd::go() {
# line 91 "SRCGCAnd.pl"
addCode(code);
}

// prototype instance for known block list
static SRCGCAnd proto;
static RegisterBlock registerBlock(proto,"And");
