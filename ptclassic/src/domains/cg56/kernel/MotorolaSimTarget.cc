static const char file_id[] = "MotorolaSimTarget.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

 Programmer: J. Buck, J. Pino

 Target for Motorola assembly code generation that runs its
 output on the simulator.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "MotorolaSimTarget.h"

void MotorolaSimTarget :: initStates(const char* dsp, const char* start, 
	const char* end) {
	dspType = dsp;
	startAddress = start;
	endAddress = end;
	addState(interactiveFlag.setState(
			"Interactive Simulation",this,"YES",""));
	addStream("simulatorCmds", &simulatorCmds);
	addStream("shellCmds", &shellCmds);
	assemblerOptions = "-A -B -L";
}

int MotorolaSimTarget::compileCode() {
	StringList assembleCmds = "asm";
	assembleCmds << dspType << " " << assemblerOptions << " " << filePrefix;
	resetMemoryUsage();
	int valid = !systemCall(assembleCmds, "Errors in assembly");
	if (valid && computeMemoryUsage()) {
	    if (int(reportMemoryUsage)) Error::message(memoryUsageString());
	}
	return valid;
}

int MotorolaSimTarget::loadCode() {
	StringList cmdFile;
	cmdFile << "load " << filePrefix << ".lod\n" << simulatorCmds
		<< "break pc>=$" << endAddress << "\ngo $" << startAddress
		<< "\n";
	if (! int(interactiveFlag)) cmdFile << "quit\n";
	return writeFile(cmdFile, ".cmd");
}

void MotorolaSimTarget::writeCode() {
    // Generate shell-cmd file for /bin/sh
    if (!parent()) {
	StringList realcmds = "#!/bin/sh\n";
	realcmds << headerComment("# ");
	realcmds << "# Remove all of the CG56WriteFile outputs\n";
	realcmds << "/bin/rm -f /tmp/cgwritefile*\n";
	realcmds << "# Run the simulator\n";
	if (int(interactiveFlag))
		realcmds << "(xterm -e sim";
	else 
		realcmds << "(sim";
	realcmds << dspType << " " << filePrefix << ".cmd" << ">/dev/null)\n";
	realcmds << "\n# Display the results\n";
	realcmds << shellCmds;
	if (!writeFile(realcmds, "", FALSE, 0755)) {
	    Error::abortRun(*this, "Failed to write the shell command file");
	    return;
	}
    }

    // generate the .asm file (and optionally display it)
    MotorolaTarget:: writeCode();
}

int MotorolaSimTarget::runCode() {
	StringList runCmd;
	runCmd << "./" << filePrefix << " &";
	return !systemCall(runCmd, "Problems running code in the simulator");
}

void MotorolaSimTarget :: headerCode () {
	simulatorCmds.initialize();
	shellCmds.initialize();
	myCode << "\torg\tp:$" << startAddress << "\nSTART\n";
};

void MotorolaSimTarget :: trailerCode () {
	trailer << "ERROR\tjmp\t$" << endAddress << "\n";
	inProgSection = TRUE;
}

void MotorolaSimTarget::frameCode()
{
    MotorolaTarget::frameCode();

    // Put a STOP instruction at the end of program memory.
    myCode << "	org	p:$" << endAddress << "\n"
	   << "	nop\n"
	   << "	stop\n";
}
