static const char file_id[] = "DynamicGalaxy.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha and J. Buck
 Date of creation: 1/14/92

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "DynamicGalaxy.h"

// Destructor body (has a different name so it can be called
// in other contexts)

void DynamicGalaxy :: zero () {

	// delete component blocks
        GalTopBlockIter nextb(*this);
        for (int i = numberBlocks(); i > 0; i--) {
                LOG_DEL; delete nextb++;
        }
	// delete multiports: must come before ports because the
	// sub-ports remove themselves from the galaxy's list.
	BlockMPHIter nextm(*this);
	for (i = numberMPHs(); i > 0; i--) {
		LOG_DEL; delete nextm++;
	}
        // delete ports
        BlockPortIter nextp(*this);
        for (i = numberPorts(); i > 0; i--) {
		PortHole* p = nextp++;
		// remove parent so it won't try to remove itself.
		p->setPort("",0);
		LOG_DEL; delete p;
        }

        // delete states
        BlockStateIter nexts(*this);
        for (i = numberStates(); i > 0; i--) {
                LOG_DEL; delete nexts++;
        }
}

// isa
ISA_FUNC(DynamicGalaxy,Galaxy);

// clone function: return error

Block* DynamicGalaxy::makeNew() const {
	Error::abortRun(*this, " cannot clone object of type 'DynamicGalaxy'");
	return 0;
}
