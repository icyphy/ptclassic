static const char file_id[] =  "DFDynPortHole.cc";
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1993 The Regents of the University of California.
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

 Programmer:  J. Buck

DFPortHole is the base class for portholes in the various dataflow
domains other than SDF that may be dynamic.

******************************************************************/

#include "DynDFPortHole.h"

inline int SorC(DFRelation rel) {
	return rel == DF_SAME || rel == DF_COMPLEMENT;
}

// setRelation sets up relationship arcs.  BDF_SAME and BDF_COMPLEMENT
// arcs always form a circle.
void DynDFPortHole::setRelation(DFRelation rel, DynDFPortHole* assocBool) {
	if (rel == relation && pAssocPort == assocBool)
		return;
	if (SorC(relation))
		removeRelation();
	relation = rel;
	pAssocPort = assocBool;
	if (!SorC(rel)) return;
	// if my peer is not part of a loop yet, create a two-port loop
	if (!SorC(assocBool->relation)) {
		assocBool->relation = relation;
		assocBool->pAssocPort = this;
		return;
	}
	// it is already part of a loop.  Insert myself between
	// assoc and assoc->pAssocPort.
	pAssocPort = assocBool->pAssocPort;
	relation = (assocBool->relation == rel) ? DF_SAME : DF_COMPLEMENT;
	assocBool->pAssocPort = this;
	assocBool->relation = rel;
}

DynDFPortHole::~DynDFPortHole() { removeRelation();}

// change my relation to DF_NONE, and delete me from any relationship
// loop.
void DynDFPortHole::removeRelation() {
	if (pAssocPort && SorC(relation)) {
		if (pAssocPort->pAssocPort == this) {
			pAssocPort->pAssocPort = 0;
			pAssocPort->relation = DF_NONE;
		}
		else {
			DynDFPortHole* p = pAssocPort;
			// go around until we get to the one that points
			// to me
			while (p->pAssocPort != this) p = p->pAssocPort;
			// make it point to the next in line instead.
			p->pAssocPort = pAssocPort;
			if (relation == DF_COMPLEMENT) {
				// reverse sense of p->relation
				p->relation = (p->relation == DF_SAME ?
					       DF_COMPLEMENT : DF_SAME);
			}
		}
	}
	relation = DF_NONE;
	pAssocPort = 0;
}

// return true if I am dynamic
int DynDFPortHole :: isDynamic() const {
	return relation == DF_TRUE || relation == DF_FALSE ||
		relation == DF_CASE || relation == DF_MULT;
}

