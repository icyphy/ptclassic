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

#include "StringList.h"
#include "CGUtilities.h"
#include "MotorolaSimTarget.h"

void MotorolaSimTarget :: initStates(
		const char* dsp, const char* start, const char* end) {
	dspType = dsp;
	startAddress = start;
	endAddress = end;
	addState(interactiveFlag.setState(
			"Interactive Simulation",this,"YES",""));
	addStream("simulatorCmds", &simulatorCmds);
	addStream("shellCmds", &shellCmds);
	assemblerOptions = "-A -B -L";
	costInfoString.initialize();
}

int MotorolaSimTarget::compileCode() {
	StringList assembleCmds = "asm";
	assembleCmds << dspType << " " << assemblerOptions << " " << filePrefix;
	resetImplementationCost();
	return !systemCall(assembleCmds, "Errors in assembly");
}

int MotorolaSimTarget::loadCode() {
	StringList cmdFile;

	// Load the assembled program
	cmdFile << "load " << filePrefix << ".lod\n" << simulatorCmds;

	// Set the ending condition
	cmdFile << "break pc>=$" << endAddress << "\n";

	// Start the simulation
	cmdFile << "go $" << startAddress << "\n";

	// Save the simulator state so that we can extract the number of
	// cycles executed (-O always overwrites the .sim if it exists)
	if (int(reportMemoryUsage)) {
	    cmdFile << "save s " << filePrefix << ".sim -O\n";
	}

	// Quit the interaction with the simulator
	if (! int(interactiveFlag)) cmdFile << "quit\n";

	return writeFile(cmdFile, ".cmd");
}

void MotorolaSimTarget::writeCode() {
    // Generate shell-cmd file for /bin/sh
    if (!parent()) {
	StringList realcmds = "#!/bin/sh\n";
	realcmds << headerComment("# ");
	realcmds << "\n# Remove all of the CG56WriteFile output files\n";
	realcmds << "/bin/rm -f /tmp/cgwritefile*\n";
	realcmds << "\n# Run the simulator\n";
	if (int(interactiveFlag))
		realcmds << "xterm -e sim";
	else 
		realcmds << "sim";
	realcmds << dspType << " " << filePrefix << ".cmd" << " >/dev/null\n";
	if (int(reportMemoryUsage)) {
		// Search the simulation state file for the pattern 'cyc:' e.g.
		// ^Break #1 pc>=$ff0 h ;dev:0 pc:0ff0 cyc:439131
		realcmds << "\n# Extract the number of cycles executed\n";
		realcmds << "/bin/rm -f " << filePrefix << ".cyc\n";
		realcmds << "grep 'cyc:' " << filePrefix << ".sim | "
			 << "sed -e 's/^.*cyc:\\([0-9]*\\)$/\\1/' >"
			 << filePrefix << ".cyc\n";
	}
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

// Run the Motorola assembly code through the simulator.
// If the user has requested to see the execution time, then
// we cannot run the simulation in the background because
// we must wait for the simulation to end before we can
// retrieve the execution time.
int MotorolaSimTarget::runCode() {
	StringList runCmd;
	runCmd << "./" << filePrefix;

	if ( int(reportMemoryUsage) ) {
	    StringList msg = "In order to extract the execution time of the ";
	    msg << filePrefix
		<< " system, user interaction will be frozen"
		<< " until the simulation completes.";
	    Error::message(*this, msg);
	}
	else {
	    runCmd << " &";
	}

	// Run the simulation
	int valid = !systemCall(runCmd, "Error running code in the simulator");

	// Extract the number of cycles executed if reportMemoryUsage is true
	if (valid && int(reportMemoryUsage)) {
	    computeImplementationCost();
	    Error::message(*this, printImplementationCost());
	}

	return valid;
}

#define MAXLINE 128

int MotorolaSimTarget::computeImplementationCost() {
	// Compute memory usage
	if (! MotorolaTarget::computeImplementationCost()) return FALSE;

	// Figure out where the .cyc file is
	// If it is on a remote machine, then copy it over
	int deleteFlag = FALSE;
	StringList cycleFileName;
	cycleFileName << filePrefix << ".cyc";
	StringList loadpathname =
		findLocalFileName(targetHost, destDirectory,
				  cycleFileName, deleteFlag);
	if ( loadpathname.length() == 0 ) return FALSE;

	// Update the execution time
	// Open the local copy of the .cyc file and read in the first
	// word as the number of cycles executed
	int retval = FALSE;
	FILE* fp = fopen(loadpathname, "r");
	if (fp) {
	    char buffer[MAXLINE];
	    int numCycles = 0;
	    ImplementationCost* costInfoPtr = implementationCost();
	    if ( fgets(buffer, MAXLINE, fp) != 0 ) {
		buffer[MAXLINE - 1] = 0;	// null terminate string
		retval = ( sscanf(buffer, "%d", &numCycles) == 1 );
	    }
	    fclose(fp);
	    costInfoPtr->setExecutionTime(numCycles);
	}

	if ( deleteFlag ) {
	    StringList removeCommand = "rm ";
	    removeCommand << loadpathname;
	    StringList errmsg = "Could not remove ";
	    errmsg << loadpathname;
	    systemCall(removeCommand, errmsg);
	}

	return retval;
}

const char* MotorolaSimTarget::printImplementationCost() {
	ImplementationCost* costInfoPtr = implementationCost();
	costInfoString = MotorolaTarget::printImplementationCost();
	if ( costInfoPtr ) {
	    costInfoString << ", execution time = "
			   << costInfoPtr->executionTime()
			   << " cycles";
	}
	return costInfoString;
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
