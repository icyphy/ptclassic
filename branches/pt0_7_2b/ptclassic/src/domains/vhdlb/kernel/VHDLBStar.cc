static const char file_id[] = "VHDLBStar.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: E. A. Lee, Asawaree Kalavade

 This is the baseclass for stars that generate VHDLB language code

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "PortHole.h"
#include "VHDLBStar.h"
#include "VHDLBTarget.h"

// The following is defined in VHDLBDomain.cc -- this forces that module
// to be included if any VHDLB stars are linked in.
extern const char VHDLBdomainName[];

const char* VHDLBStar :: domain () const { return VHDLBdomainName;}

// isa

ISA_FUNC(VHDLBStar, CGStar);

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

StringList VHDLBStar::expandRef(const char* name, const char* offset) 
{
        codeblockError("Use @ operator instead of expandRef(,) syntax.");
        return "ERROR";
}

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
