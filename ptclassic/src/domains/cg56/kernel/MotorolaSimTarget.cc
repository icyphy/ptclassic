static const char file_id[] = "MotorolaSimTarget.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck, J. Pino

 Target for Motorola assembly code generation that runs its
 output on the simulator.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "MotorolaSimTarget.h"

void MotorolaSimTarget :: initStates(const char* dsp,const char* start, 
	const char* end) {
	dspType = dsp;
	startAddress = start;
	endAddress = end;
	addState(plotFile.setState("Plot file name",this,"outfile.sim",
		"file to plot with xgraph after run"));
	addState(plotTitle.setState("Plot title",this,"Simulator output",
		"graph title (if any)"));
	addState(plotOptions.setState("Plot options",this,"",
		"xgraph options"));
	addState(interactiveFlag.setState(
		"Interactive Simulation",this,"YES",""));
	addStream("simulatorCmds",&simulatorCmds);
	runFlag.setInitValue("YES");
	runFlag.setAttributes(A_SETTABLE|A_NONCONSTANT);
}

int MotorolaSimTarget::compileCode() {
	StringList assembleCmds;
	assembleCmds << "asm" << dspType << " -A -b -l " << uname;
	return !systemCall(assembleCmds,"Errors in assembly");
}

int MotorolaSimTarget::loadCode() {
	const char* file = plotFile;
	if (*file != 0) unlink(fullFileName(file));
	StringList cmdFile;
	cmdFile << "load " << uname << ".lod\n" << simulatorCmds
		<< "break pc>=$" << endAddress << "\ngo $" << startAddress
		<< "\n";
	if (!interactiveFlag) cmdFile << "quit\n";
	return genFile(cmdFile, uname,".cmd");
}

int MotorolaSimTarget::runCode() {
	const char* file = plotFile;
	StringList downloadCmds;
	if (interactiveFlag) 
		downloadCmds << "(xterm -e sim";
	else 
		downloadCmds << "(sim";
	downloadCmds << dspType << " " << uname << ".cmd" << " > /dev/null)";
	if (systemCall(downloadCmds,"Errors in starting up the simulator")) 
		return FALSE;
	if (*file != 0 && access(fullFileName(file),0)==0) {
		StringList plotCmds;
 		plotCmds << "awk '{print ++n, $1}' " << file
		  << " | xgraph -t '" << (const char*)plotTitle << "' "
		  << (const char*)plotOptions << "&\n";
		return !systemCall(plotCmds,"Plot unsuccessful");
	}
	return TRUE;
}

void MotorolaSimTarget :: headerCode () {
	simulatorCmds.initialize();
	myCode << "	org	p:$" << startAddress << "\nSTART\n";
};

void MotorolaSimTarget :: trailerCode () {
	myCode << "	jmp	$" << endAddress << "\n"
               <<"ERROR	jmp	$" << endAddress << "\n"
	       << "	org	p:$" << endAddress << "\n"
	       << "	nop\n"
	       << "	stop\n";
	inProgSection = TRUE;
}
