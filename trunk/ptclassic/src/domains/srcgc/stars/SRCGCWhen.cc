static const char file_id[] = "SRCGCWhen.pl";
// .cc file generated from SRCGCWhen.pl by ptlang
/*
Copyright (c) 1998 Supélec & Thomson-CSF Optronique.
    All rights reserved.
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "SRCGCWhen.h"

const char *star_nm_SRCGCWhen = "SRCGCWhen";

const char* SRCGCWhen :: className() const {return star_nm_SRCGCWhen;}

ISA_FUNC(SRCGCWhen,SRCGCNonStrictStar);

Block* SRCGCWhen :: makeNew() const { LOG_NEW; return new SRCGCWhen;}

CodeBlock SRCGCWhen :: code (
# line 78 "SRCGCWhen.pl"
"    if ( !SRCGC_ISKNOWN($ref(output)) && SRCGC_ISKNOWN($ref(clock)) ) {\n"
"      if SRCGC_ISPRESENT($ref(clock)) {\n"
"        if ( SRCGC_GET($ref(clock)) ) {\n"
"          /* clock is present and true--copy input to output */\n"
"          if ( SRCGC_ISKNOWN($ref(input)) ) {\n"
"            if ( SRCGC_ISPRESENT($ref(input)) ) { \n"
"              /* input is present--copy it */ \n"
"              SRCGC_EMIT($ref(output),SRCGC_GET($ref(input)));\n"
"            } else {\n"
"              // input is absent\n"
"              SRCGC_MAKEABSENT($ref(output));\n"
"            }\n"
"          }\n"
"        } else {\n"
"          // clock is present and false\n"
"          SRCGC_MAKEABSENT($ref(output));\n"
"        }\n"
"      } else {\n"
"        // clock is absent--make the output absent\n"
"        SRCGC_MAKEABSENT($ref(output));\n"
"      }\n"
"    }   \n");

SRCGCWhen::SRCGCWhen ()
{
	setDescriptor("When the clock input is true, copy the input to the output;\n    when the clock is absent, make the output absent.");
	addPort(input.setPort("input",this,INT));
	addPort(clock.setPort("clock",this,INT));
	addPort(output.setPort("output",this,INT));

# line 75 "SRCGCWhen.pl"
noInternalState();
    reactive();
}

int SRCGCWhen::myExecTime() {
# line 107 "SRCGCWhen.pl"
return 0;
}

void SRCGCWhen::go() {
# line 104 "SRCGCWhen.pl"
addCode(code);
}

// prototype instance for known block list
static SRCGCWhen proto;
static RegisterBlock registerBlock(proto,"When");
