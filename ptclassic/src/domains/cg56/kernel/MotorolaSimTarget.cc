static const char file_id[] = "MotorolaSimTarget.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Pino and J. Buck

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "MotorolaAttributes.h"
#include "AsmStar.h"

class MotorolaSimTarget :: initStates() {
	addState(plotFile.setState("plotFile",this,"outfile.sim",
		"file to plot with xgraph after run"));
	addState(plotTitle.setState("plotTitle",this,"Simulator output", 
		"graph title (if any)"));
	addState(plotOptions.setState("plotOptions",this,"",
		"xgraph options"));
	addState(intSim.setState("Interactive Simulation?",this,"YES"));
	runCode.setValue("YES");
	runCode.setAttributes(A_SETTABLE|A_NONCONSTANT);
}

class MotorolaSimTarget :: initializeCmds(const char* dspType) {
	StringList outfile = dirFullName;
	outfile += "/";
	outfile += plotFile.currentValue();
	unlink(outfile);
	CG96Target::initializeCmds();
	assembleCmds += "asm";
	assembleCmds += dspType;
	assembleCmds += "000 -A -b -l ";
	assembleCmds += fileName(uname,".asm");
	assembleCmds += "\n";
	miscCmds += "load ";
	miscCmds += uname;
	miscCmds += "\n";
	if (int(intSim) == TRUE) 
		downloadCmds += "xterm -e sim96000 ";
	else
		downloadCmds += "sim96000 ";
	downloadCmds += fileName(uname,".cmd\n");
	const char* file = plotFile;
	if (*file != 0) {
		downloadCmds += "awk '{print ++n, $1}' ";
		downloadCmds += file;
		downloadCmds += " | xgraph -t '";
		downloadCmds += (const char*)plotTitle;
		downloadCmds += "' ";
		downloadCmds += (const char*)plotOptions;
		downloadCmds += "&\n";
	}
}

int MotorolaSimTarget :: wrapup(const char* startAddress, const char* finalJumpLocation) {
	StringList wrapupCode = "	jmp	";
	wrapupCode += finalJumpLocation;
	wrapupCode += "\nERROR	jmp	";
	wrapupCode += finalJumpLocation;
	wrapupCode += "\n	org	p:";
	wrapupCode += finalJumpLocation;
	wrapupCode += "\n	nop\n	stop\n";
	miscCmds += "break pc>=";
	miscCmds += finalJumpLocation;
	miscCmds += "\ngo ";
	miscCmds += startAddress;
	miscCmds += "\n";
	if (!int(intSim)) miscCmds += "quit\n";
	inProgSection = TRUE;
	return genFile(miscCmds, uname,".cmd"));
}
