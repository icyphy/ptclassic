static const char file_id[] = "DynamicGalaxy.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 1/14/90

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "DynamicGalaxy.h"

/*******************************************************************

	Destructor

********************************************************************/

DynamicGalaxy :: ~DynamicGalaxy() {

	// delete component blocks
        GalTopBlockIter nextb(*this);
        for (int i = numberBlocks(); i > 0; i--) {
                LOG_DEL; delete nextb++;
        }
        // delete ports
        BlockPortIter nextp(*this);
        for (i = numberPorts(); i > 0; i--) {
                LOG_DEL; delete nextp++;
        }

        // delete states
        BlockStateIter nexts(*this);
        for (i = numberStates(); i > 0; i--) {
                LOG_DEL; delete nexts++;
        }
}

// isa
ISA_FUNC(DynamicGalaxy,Galaxy);
