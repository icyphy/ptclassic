/**********************************************************************
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

 Programmer:       Bilung Lee
 Date of creation: 3/3/96

 Declaration for FSMTarget.

***********************************************************************/
#ifndef _FSMTarget_h
#define _FSMTarget_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "StringState.h"
#include "FloatState.h"
#include "Block.h"
#include "Target.h"

class FSMTarget : public Target {

public:
	// constructor
	FSMTarget();

	// destructor
	~FSMTarget();

	void setup();
	void begin();

	// return a copy of itself
	Block* makeNew() const;

	// return the domain of the galaxy and "FSM" otherwise
	/*virtual*/ const char* domain();

protected:
	// Input name maps
        StringState inputNameMap;         

	// Output name maps
	StringState outputNameMap;   

	// Internal event name maps
	StringState internalNameMap;   

        // Machine type: Pure.
	StringState machineType;

	// Evaluation type: Strict or NonStrict.
	StringState evaluationType;

	// One-writer rule checking type: Compile Run None.
	StringState oneWriterType;

	// schedulePeriod for interface with a timed domain.
	FloatState schedulePeriod;

};
#endif
