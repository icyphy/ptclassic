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
#ifndef _MotorolaSimTarget_h
#define _MotorolaSimTarget_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "MotorolaTarget.h"
#include "StringState.h"
#include "IntState.h"

class MotorolaSimTarget : public virtual MotorolaTarget {
protected:
	StringState plotFile;
	StringState plotTitle;
	StringState plotOptions;
	IntState interactiveFlag;
	StringList dspType;	//either 56000 or 96000
	StringList startAddress;
	StringList endAddress;
	/*virtual*/ void headerCode();
	/*virtual*/ void trailerCode();
public:
	MotorolaSimTarget(const char* nam,const char* desc,
		  const char* sclass) : MotorolaTarget(nam,desc,sclass) {}
	MotorolaSimTarget(const MotorolaSimTarget& arg) 
	: MotorolaTarget(arg) {}
	void initStates(const char* dsp,const char* start, const char* end);
	int compileCode();
	int loadCode();
	int runCode();
private:
	// stream for writeFile stars
	CodeStream simulatorCmds;
};

#endif
