#ifndef _VHDLBStar_h
#define _VHDLBStar_h 1
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

 Programmer: Edward A. Lee and J. T. Buck

 This is the baseclass for stars that generate VHDLB language code

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "CGStar.h"
#include "VHDLBPortHole.h"
#include "SDFPortHole.h"
#define VHDLBCodeBlock CodeBlock

class VHDLBTarget;

class VHDLBStar : public CGStar {
friend class VHDLBTarget;
private:
	int forkId;

protected:
        virtual const char* portWidthName(const MultiVHDLBPort* pp);

	void setForkId() { forkId = TRUE; }

	// If "name" is a state, add the state to the list of referenced
	// states.  Then check to see whether name is a PortHole. If so,
	// check to see whether the PortHole is aliased to a Galaxy port.
	// If so, return the name of the Galaxy port.  If not,
	// get the reference from the geodesic.
	// If "name" is neither a PortHole nor a State, invoke
	// same method in CGStar.

        virtual StringList expandRef(const char* name);
/*
        virtual StringList expandRef(const char* name, const char* offset);
*/

	// If "name" is a state, and is not already on the list
	// referencedStates, add it.
	void registerState(const char* name);

	// Add lines to be put at the beginning of the code file
	void addInclude(const char* decl);

	// Add global declarations, to be put ahead of the main section
	void addGlobal(const char* decl);

	// Add global declarations, to be put ahead of the main section
	void addMainInit(const char* decl);

         // access to target
         VHDLBTarget* target() { return (VHDLBTarget*)target(); }



public:
	VHDLBStar(): forkId(0) {}

	// List of all states pointed to in the code.
	// This is public so that VHDLBTarget and other targets can access it.
	StateList referencedStates;

	// start method clears out referencedStates list
	void initialize();

	// my domain
	const char* domain() const;

	int run();

	// class identification
	int isA(const char*) const;

	// fork star identification
	int amIFork() { return forkId; }

};
#endif
