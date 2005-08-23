static const char file_id[] = "SRCGCMux.pl";
// .cc file generated from SRCGCMux.pl by ptlang
/*
Copyright (c) 1998 Supélec & Thomson-CSF Optronique.
    All rights reserved.
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "SRCGCMux.h"

const char *star_nm_SRCGCMux = "SRCGCMux";

const char* SRCGCMux :: className() const {return star_nm_SRCGCMux;}

ISA_FUNC(SRCGCMux,SRCGCNonStrictStar);

Block* SRCGCMux :: makeNew() const { LOG_NEW; return new SRCGCMux;}

CodeBlock SRCGCMux :: code (
# line 83 "SRCGCMux.pl"
"    if ( !SRCGC_ISKNOWN($ref(output)) && SRCGC_ISKNOWN($ref(select)) ) {\n"
"      if ( SRCGC_ISPRESENT($ref(select)) ) {\n"
"        if  ( SRCGC_GET($ref(select)) ) {\n"
"          // Select is true--copy the true input if it's known        \n"
"          if ( SRCGC_ISKNOWN($ref(trueInput)) ) {\n"
"            if ( SRCGC_ISPRESENT($ref(trueInput)) ) {\n"
"              SRCGC_EMIT($ref(output),SRCGC_GET($ref(trueInput)));\n"
"            } else {\n"
"              // true input is absent: make the output absent\n"
"              SRCGC_MAKEABSENT($ref(output));\n"
"            }\n"
"          }\n"
"        } else {\n"
"          // Select is false--copy the false input if it's known        \n"
"          if ( SRCGC_ISKNOWN($ref(falseInput)) ) {\n"
"            if ( SRCGC_ISPRESENT($ref(falseInput)) ) {\n"
"              SRCGC_EMIT($ref(output),SRCGC_GET($ref(falseInput))); \n"
"            } else {\n"
"              // false input is absent: make the output absent\n"
"              SRCGC_MAKEABSENT($ref(output));\n"
"            }\n"
"          }\n"
"        } \n"
"      }else {\n"
"        // Select is absent: make the output absent\n"
"        SRCGC_MAKEABSENT($ref(output));\n"
"      }\n"
"    }\n");

SRCGCMux::SRCGCMux ()
{
	setDescriptor("When the select input is true, copy trueInput to the output;\n    when the select input is false, copy falseInput to the output;\n    make the output absent when the select input is absent.");
	addPort(trueInput.setPort("trueInput",this,INT));
	addPort(falseInput.setPort("falseInput",this,INT));
	addPort(select.setPort("select",this,INT));
	addPort(output.setPort("output",this,INT));

# line 80 "SRCGCMux.pl"
noInternalState();
    reactive();
}

void SRCGCMux::go() {
# line 114 "SRCGCMux.pl"
addCode(code);
}

// prototype instance for known block list
static SRCGCMux proto;
static RegisterBlock registerBlock(proto,"Mux");
