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

void MotorolaSimTarget :: initStates(const char* dsp,const char* start, 
	const char* end) {
	dspType = dsp;
	startAddress = start;
	endAddress = end;
	addState(interactiveFlag.setState(
		"Interactive Simulation",this,"YES",""));
	addStream("simulatorCmds",&simulatorCmds);
	addStream("shellCmds",&shellCmds);
}

int MotorolaSimTarget::compileCode() {
	StringList assembleCmds;
	assembleCmds << "asm" << dspType << " -A -b -l " << filePrefix;
	return !systemCall(assembleCmds,"Errors in assembly");
}

int MotorolaSimTarget::loadCode() {
	StringList cmdFile;
	cmdFile << "load " << filePrefix << ".lod\n" << simulatorCmds
		<< "break pc>=$" << endAddress << "\ngo $" << startAddress
		<< "\n";
	if (!interactiveFlag) cmdFile << "quit\n";
	return writeFile(cmdFile,".cmd");
}

void MotorolaSimTarget::writeCode() {
    /*
     * generate shell-cmd file (/bin/sh)
     */
    if (!parent()) {
	StringList realcmds = "#!/bin/sh\n";
	realcmds << headerComment("# ");
	realcmds << "# Remove all of the CG56WriteFile outputs\n";
	realcmds << "/bin/rm -f /tmp/cgwritefile*\n";
	realcmds << "# Run the simulator\n";
	if (interactiveFlag) 
		realcmds << "(xterm -e sim";
	else 
		realcmds << "(sim";
	realcmds << dspType << " " << filePrefix << ".cmd" << ">/dev/null)\n";
	realcmds << "\n# Display the results\n";
	realcmds << shellCmds;
	if (!writeFile(realcmds,"",FALSE,0755)) {
	    Error::abortRun(*this,"Shell command file write failed");
	    return;
	}
    }
    /*
     * generate the .asm file (and optionally display it)
     */
    MotorolaTarget:: writeCode();
}

int MotorolaSimTarget::runCode() {
	StringList runCmd;
	runCmd << "./" << filePrefix << " &";
	if (systemCall(runCmd,"Problems running code onto simulator")!=0)
	    return FALSE;
	return TRUE;
}

void MotorolaSimTarget :: headerCode () {
	simulatorCmds.initialize();
	shellCmds.initialize();
	myCode << "	org	p:$" << startAddress << "\nSTART\n";
};

void MotorolaSimTarget :: trailerCode () {
	trailer << "ERROR	jmp	$" << endAddress << "\n";
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
