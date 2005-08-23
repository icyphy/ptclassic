/******************************************************************
Version identification:
@(#)Sim56Target.h	1.18	03/21/97

Copyright (c) 1990-1997 The Regents of the University of California.
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

 Target for Motorola 56000 assembly code generation that runs its
 output on the simulator.

*******************************************************************/
#ifndef _Sim56Target_h
#define _Sim56Target_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "CG56Target.h"
#include "MotorolaSimTarget.h"

class Sim56Target :public CG56Target, public MotorolaSimTarget {
public:
	// Constructor
	Sim56Target(const char* name, const char* desc,
		    const char* assocDomain = CG56domainName);

	// Copy constructor
	Sim56Target(const Sim56Target&);

	/*virtual*/ int compileCode();
	/*virtual*/ int loadCode();
	/*virtual*/ int runCode();
	/*virtual*/ void writeCode();
	/*virtual*/ void frameCode() { MotorolaSimTarget::frameCode(); }

	// Return a copy of itself
	/*virtual*/ Block* makeNew() const;

	// Type hierarchy checking
	int isA(const char*) const;

        // Workaround for SGI Compiler Problem
        int computeImplementationCost()
          { return MotorolaSimTarget::computeImplementationCost(); }

        const char* describeImplementationCost()
          { return MotorolaSimTarget::describeImplementationCost(); }

protected:
        /*virtual*/ void headerCode();
        /*virtual*/ void trailerCode();

private:
	void initStates();
};

#endif
