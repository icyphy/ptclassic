static const char file_id[] = "MotorolaSimTarget.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.
							COPYRIGHTENDKEY

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
