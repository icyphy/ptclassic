static const char file_id[] = "VHDLLFCReceive.pl";
// .cc file generated from VHDLLFCReceive.pl by ptlang
/*
Copyright (c) 1993-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "VHDLLFCReceive.h"

const char *star_nm_VHDLLFCReceive = "VHDLLFCReceive";

const char* VHDLLFCReceive :: className() const {return star_nm_VHDLLFCReceive;}

ISA_FUNC(VHDLLFCReceive,VHDLCSynchComm);

Block* VHDLLFCReceive :: makeNew() const { LOG_NEW; return new VHDLLFCReceive;}

CodeBlock VHDLLFCReceive :: C2VintegerConfig (
# line 32 "VHDLLFCReceive.pl"
"for all:C2Vinteger use entity PTVHDLSIM.C2Vinteger(CLI); end for;\n");

CodeBlock VHDLLFCReceive :: C2VrealConfig (
# line 36 "VHDLLFCReceive.pl"
"for all:C2Vreal use entity PTVHDLSIM.C2Vreal(CLI); end for;\n");

VHDLLFCReceive::VHDLLFCReceive ()
{
	setDescriptor("CGC to VHDL Receive star");
	addPort(output.setPort("output",this,ANYTYPE));


}

void VHDLLFCReceive::begin() {
# line 42 "VHDLLFCReceive.pl"
// FIXME: This is a hack to get around unsynthesizable code.
    if(!(targ()->isA("SynthTarget"))) {

    if (strcmp(output.resolvedType(), "INT") == 0) {
      addCode(C2VintegerConfig, "cli_configs", "c2vintconfig");
    }
    else if (strcmp(output.resolvedType(), "FLOAT") == 0) {
      addCode(C2VrealConfig, "cli_configs", "c2vrealconfig");
    }
    else
      Error::abortRun(*this, output.resolvedType(), ": type not supported");

    }
}

void VHDLLFCReceive::setup() {
# line 21 "VHDLLFCReceive.pl"
if (strcmp(output.resolvedType(), "INT") == 0) 
      rtype = "integer";
    else if (strcmp(output.resolvedType(), "FLOAT") == 0) 
      rtype = "real";
    else
      Error::abortRun(*this, output.resolvedType(), ": type not supported");

    numXfer = output.numXfer();     
    VHDLCSynchComm::setup();
}

void VHDLLFCReceive::go() {
# line 58 "VHDLLFCReceive.pl"
// FIXME: This is a hack to get around unsynthesizable code.
    if(!(targ()->isA("SynthTarget"))) {

    // Added this in here instead of in begin().
    int direction = 0;
    targ()->registerComm(direction, int(pairNumber), numXfer,
			 output.resolvedType());

    // Add code to synch at beginning of main.
    StringList dataSynch;
    for (int i = 0 ; i < numXfer ; i++) {
      dataSynch << "C2V" << rtype << int(pairNumber) << "_go" << " <= '0';\n";
      dataSynch << "wait on " << "C2V" << rtype << int(pairNumber) << "_done"
		<< "'transaction;\n";
      dataSynch << "$ref(output,";
      dataSynch << -i;
      dataSynch << ") $assign(output) " << "C2V" << rtype << int(pairNumber) << "_data;\n";
    }
    
    addCode(dataSynch);

    }
}

// prototype instance for known block list
static VHDLLFCReceive proto;
static RegisterBlock registerBlock(proto,"LFCReceive");
