static const char file_id[] = "VHDLBStar.cc";
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

 Programmer: Edward A. Lee, Asawaree Kalavade

 This is the baseclass for stars that generate VHDLB language code

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "PortHole.h"
#include "VHDLBStar.h"
#include "VHDLBTarget.h"
#include "State.h"
#include "Fix.h"

// The following is defined in VHDLBDomain.cc -- this forces that module
// to be included if any VHDLB stars are linked in.
extern const char VHDLBdomainName[];

const char* VHDLBStar :: domain () const { return VHDLBdomainName;}

// isa

ISA_FUNC(VHDLBStar, CGStar);

const char* VHDLBStar::portWidthName(const MultiVHDLBPort* pp) {
  Error::abortRun(*this, pp->print(), ": Must redefine portWidthName");
  return pp->print();
}

StringList VHDLBStar::expandRef(const char* name) 
{
    StringList ref;
    VHDLBPortHole* port;
    State* st;
    StringList portName = expandPortName(name);

    if ((st = stateWithName(name)) != NULL)
    {
        registerState(name);
        ref << target()->correctName(*st);
    }
    else if ((port = (VHDLBPortHole*)genPortWithName(portName)) != NULL)
    {

        ref << port->getGeoReference();
    }
    else
    {
        codeblockError(name, " is not defined as a state or port");
        ref.initialize();
    }
    return ref;

}

/*
StringList VHDLBStar::expandRef(const char* name, const char* offset) 
{
        codeblockError("Use @ operator instead of expandRef(,) syntax.");
        return "ERROR";
}
*/

void VHDLBStar::registerState(const char* name) {
	State* state;
	if(state = stateWithName(name)) {
		// If the state is not already on the list of referenced
		// states, add it.
		StateListIter nextState(referencedStates);
		const State* sp;
		while ((sp = nextState++) != 0)
			if(strcmp(name,sp->name()) == 0) return;
		referencedStates.put(*state);
	}
}

/*
void VHDLBStar::addInclude(const char* decl) {
	((VHDLBTarget*)target())->addInclude(decl);
}

void VHDLBStar::addGlobal(const char* decl) {
	((VHDLBTarget*)target())->addGlobal(decl);
}

void VHDLBStar::addMainInit(const char* decl) {
	((VHDLBTarget*)target())->addMainInit(decl);
}
*/

void VHDLBStar::initialize() {
	CGStar::initialize();
	referencedStates.initialize();
}

// fire: prefix the code with a comment

int VHDLBStar::run() {
	StringList MyCode = "    /* firing of star ";
	MyCode += fullName();
	MyCode += " (class ";
	MyCode += className();
	MyCode += ") */\n";
	addCode(MyCode);
	int status = CGStar::run();
	return status;
}
