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
	runFlag.setValue("YES");
	runFlag.setAttributes(A_SETTABLE|A_NONCONSTANT);
}

int MotorolaSimTarget::compileCode() {
	StringList assembleCmds  = "asm";
	assembleCmds += dspType;
	assembleCmds += " -A -b -l ";
	assembleCmds += uname;
	return !systemCall(assembleCmds,"Errors in assembly");
}

int MotorolaSimTarget::loadCode() {
	const char* file = plotFile;
	if (*file != 0) unlink(fullFileName(file));
	StringList cmdFile = "load ";
	cmdFile += fileName(uname,".lod\n");
	cmdFile += miscCmds;
	cmdFile += "break pc>=$";
	cmdFile += endAddress;
	cmdFile += "\ngo $";
	cmdFile += startAddress;
	cmdFile += "\n";
	if (!interactiveFlag) cmdFile += "quit\n";
	if (!genFile(cmdFile, uname,".cmd")) return FALSE;
	return TRUE;
}

int MotorolaSimTarget::runCode() {
	const char* file = plotFile;
	StringList downloadCmds;
	if (interactiveFlag) 
		downloadCmds = "(xterm -e sim";
	else 
		downloadCmds = "(sim";
	downloadCmds += dspType;
	downloadCmds +=" ";
	downloadCmds += fileName(uname,".cmd");
	downloadCmds += " > /dev/null)";
	if (systemCall(downloadCmds,"Errors in starting up the simulator")) 
		return FALSE;
	if (*file != 0 && access(fullFileName(file),0)==0) {
		StringList plotCmds;
 		plotCmds = "awk '{print ++n, $1}' ";
		plotCmds += file;
		plotCmds += " | xgraph -t '";
		plotCmds += (const char*)plotTitle;
		plotCmds += "' ";
		plotCmds += (const char*)plotOptions;
		plotCmds += "&\n";
		return !systemCall(plotCmds,"Plot unsuccessful");
	}
	return TRUE;
}

void MotorolaSimTarget :: headerCode () {
	StringList code  = "	org	p:$";
	code += startAddress;
	code += "\nSTART\n";
	addCode(code);
};

void MotorolaSimTarget :: wrapup () {
	StringList code = "	jmp	$";
	code += endAddress;
	code += "\nERROR	jmp	$";
	code += endAddress;
	code += "\n	org	p:$";
	code += endAddress;
	code += "\n	nop\n	stop\n";
	inProgSection = TRUE;
	addCode(code);
}
