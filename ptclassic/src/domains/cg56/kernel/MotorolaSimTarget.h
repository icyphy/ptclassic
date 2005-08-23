/******************************************************************
Version identification:
@(#)MotorolaSimTarget.h	1.15	7/30/96

Copyright (c) 1990-1996 The Regents of the University of California.
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
#ifndef _MotorolaSimTarget_h
#define _MotorolaSimTarget_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "StringList.h"
#include "StringState.h"
#include "IntState.h"
#include "MotorolaTarget.h"

class MotorolaSimTarget : public virtual MotorolaTarget {
public:
	MotorolaSimTarget(const char* nam, const char* desc,
			  const char* sclass, const char* assocDomain) :
		MotorolaTarget(nam,desc,sclass,assocDomain) {}
	MotorolaSimTarget(const MotorolaSimTarget& arg) :
		MotorolaTarget(arg) {}

	void initStates(const char* dsp, const char* start, const char* end);

	/*virtual*/ int compileCode();
	/*virtual*/ int loadCode();
	/*virtual*/ int runCode();
	/*virtual*/ void writeCode();
	/*virtual*/ void frameCode();

	int computeImplementationCost();
	const char* describeImplementationCost();

        // Indicate whether target can compute execution time
	int canComputeExecutionTime() { return TRUE; }

protected:
	IntState interactiveFlag;
	StringList dspType;		// either 56000 or 96000
	StringList startAddress;
	StringList endAddress;
	/*virtual*/ void headerCode();
	/*virtual*/ void trailerCode();

private:
	// stream for writeFile stars
	CodeStream simulatorCmds;
	CodeStream shellCmds;

	// implementation costs
	StringList costInfoString;

	// return the execution time
	int computeExecutionTime();
};

#endif
