static const char file_id[] = "DFNebula.cc";
/******************************************************************
Version identification:
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

 Programmer: Jose Luis Pino
 Date of creation: 6/10/94

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "DFNebula.h"

int DFNebula::run() {
    return Nebula::run();
}

// Constructors
DFNebula::DFNebula() : DataFlowStar(), Nebula(*(DataFlowStar*)this) {};

DFNebulaPort::DFNebulaPort(const PortHole* master, Star* parent)
: DFPortHole(), NebulaPort(*this,*master,parent) {
    const DFPortHole& dfmaster = *(const DFPortHole*)master;
    DFPortHole::maxBackValue = dfmaster.maxDelay();
}

PortHole* DFNebula::clonePort(const PortHole* master, Star* parent) {
    return new DFNebulaPort(master,parent);
}

Nebula* DFNebula::newNebula(Block* master) const {
    LOG_NEW; Nebula* neb = new DFNebula();
    if (master) neb->setMasterBlock(master);
    return neb;
}

int DFNebula::isSDFinContext() const {
    
    if (isNebulaAtomic())
	return Nebula::master?((DataFlowStar*)Nebula::master)->isSDFinContext():TRUE;
    GalStarIter nextStar(gal);
    DataFlowStar* s;
    while ((s = (DataFlowStar*) nextStar++) != 0)
	if (!s->isSDFinContext()) return FALSE;
    return TRUE;
}

/*virtual*/ void DFNebula::setMasterBlock(Block*master,PortHole**newPorts) {
    Nebula::setMasterBlock(master,newPorts);
    if (master->isItAtomic())
	this->repetitions = ((DataFlowStar*)master)->repetitions;
}



