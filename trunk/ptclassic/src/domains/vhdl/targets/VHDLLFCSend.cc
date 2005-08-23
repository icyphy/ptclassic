static const char file_id[] = "VHDLLFCSend.pl";
// .cc file generated from VHDLLFCSend.pl by ptlang
/*
Copyright (c) 1993-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright	for copyright notice,
limitation of liability, and disclaimer	of warranty provisions.
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLLFCSend.h"

const char *star_nm_VHDLLFCSend = "VHDLLFCSend";

const char* VHDLLFCSend :: className() const {return star_nm_VHDLLFCSend;}

ISA_FUNC(VHDLLFCSend,VHDLCSynchComm);

Block* VHDLLFCSend :: makeNew() const { LOG_NEW; return new VHDLLFCSend;}

CodeBlock VHDLLFCSend :: V2CintegerConfig (
# line 32 "VHDLLFCSend.pl"
"for all:V2Cinteger use entity PTVHDLSIM.V2Cinteger(CLI); end for;\n");

CodeBlock VHDLLFCSend :: V2CrealConfig (
# line 36 "VHDLLFCSend.pl"
"for all:V2Creal use entity PTVHDLSIM.V2Creal(CLI); end for;\n");

VHDLLFCSend::VHDLLFCSend ()
{
	setDescriptor("VHDL to CGC synchronous send star");
	addPort(input.setPort("input",this,ANYTYPE));


}

void VHDLLFCSend::begin() {
# line 42 "VHDLLFCSend.pl"
// FIXME: This is a hack to get around unsynthesizable code.
    if(!(targ()->isA("SynthTarget"))) {

    if (strcmp(input.resolvedType(), "INT") == 0) {
      addCode(V2CintegerConfig, "cli_configs", "v2cintconfig");
    }
    else if (strcmp(input.resolvedType(), "FLOAT") == 0) {
      addCode(V2CrealConfig, "cli_configs", "v2crealconfig");
    }
    else
      Error::abortRun(*this, input.resolvedType(), ": type not supported");

    }
}

void VHDLLFCSend::setup() {
# line 21 "VHDLLFCSend.pl"
if (strcmp(input.resolvedType(), "INT") == 0) 
      rtype = "integer";
    else if (strcmp(input.resolvedType(), "FLOAT") == 0) 
      rtype = "real";
    else
      Error::abortRun(*this, input.resolvedType(), ": type not supported");

    numXfer = input.numXfer();
    VHDLCSynchComm::setup();
}

void VHDLLFCSend::go() {
# line 58 "VHDLLFCSend.pl"
// FIXME: This is a hack to get around unsynthesizable code.
    if(!(targ()->isA("SynthTarget"))) {

    // Added this in here instead of in begin().
    int direction = 1;
    targ()->registerComm(direction, int(pairNumber), numXfer,
			 input.resolvedType());

    // Add code to synch at end of main.
    StringList dataSynch;
    for (int i = 0 ; i < numXfer ; i++) {
      dataSynch << "V2C" << rtype << int(pairNumber) << "_data" << " <= "
		<< "$ref(input,";
      dataSynch << -i;
      dataSynch << ")" << ";\n";
      dataSynch << "V2C" << rtype << int(pairNumber) << "_go" << " <= "
		<< "'0';\n";
      dataSynch << "wait on " << "V2C" << rtype << int(pairNumber) << "_done"
		<< "'transaction;\n";
    }
    
    addCode(dataSynch);

    }
}

// prototype instance for known block list
static VHDLLFCSend proto;
static RegisterBlock registerBlock(proto,"LFCSend");
