#ifndef _SDFWormhole_h
#define _SDFWormhole_h 1
#ifdef __GNUG__
#pragma interface
#endif

/*******************************************************************
 SCCS Version identification :
 $Id$

Copyright (c) 1990-1994 The Regents of the University of California.
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

 Programmer : Jose Luis Pino
 Date of Creation : 6/10/90
	
********************************************************************/

#include "Nebula.h"
#include "DataFlowStar.h"
#include "DFPortHole.h"

class DFNebulaPort : public NebulaPort, public DFPortHole {
public:
	DFNebulaPort(const PortHole* master,
		     const Nebula* parent);
	/*virtual*/ int isItInput() {
		return NebulaPort::isItInput();
	}
	/*virtual*/ int isItOutput() {
		return NebulaPort::isItOutput();
	}
};

class DFNebula : public Nebula, public DataFlowStar {
public:
	/*virtual*/ int run();

	// Constructors
	DFNebula(Block* g);

	/*virtual*/ PortHole*
	clonePort(const PortHole* master) const {
		return new DFNebulaPort(master,this);
	}

	/*virtual*/ Nebula* newNebula(Block* master) const {
		return new DFNebula(master);
	}
};
#endif





