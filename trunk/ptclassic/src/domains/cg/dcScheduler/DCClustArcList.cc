static const char file_id[] = "DCClustArcList.cc";
/*****************************************************************
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

Programmer: G.C. Sih and Soonhoi Ha
Date of last revision: 5/92 

*****************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "DCClustArcList.h"
#include "DCCluster.h"

StringList DCClustArc::print() {
	StringList out;
	out += "(DCCluster ";
	out += getNeighbor()->readName();
	out += ", ";
	out += getSamples();
	out += ")\n";
	return out;
}

//------------------------------------------------------------------

			///////////////////
			///  changeArc  ///
			///////////////////
// Changes the neighbor pointer to a new cluster.  
// after a remove() call, so we re-scan the loop from the beginning.

void DCClustArcList::changeArc(DCCluster *oldname, DCCluster *newname) {

	// check whether there is an arc connected to the newname.
	DCClustArc* ref = contain(newname);

	DCClustArcListIter iter(*this);
	DCClustArc *arc;
	int change;
	do {
		iter.reset();
		change = FALSE;
		while ((arc = iter++) != 0) {
			if (arc->getNeighbor() == oldname) {
				if (ref == 0) {
					arc->changeNeighbor(newname);
					ref = arc;
				} else {
					ref->addSamples(arc->getSamples());
					// Remove the arc
					remove(arc);
					break;
				}
				change = TRUE;
			}
		}
	} while (change);	// end loop if we got through with no changes
}

// check whether the given cluster is already known as a neighbor or not.
DCClustArc* DCClustArcList :: contain(DCCluster* c) {
	DCClustArcListIter citer(*this);
	DCClustArc* carc;

	while ((carc = citer++) != 0) {
		if (carc->getNeighbor() == c) return carc;
	}
	return 0;
}

StringList DCClustArcList::print() {
	StringList out;
	DCClustArcListIter iter(*this);
	DCClustArc *arc;

	while ((arc = iter++) != 0) {
		out += arc->print();
	}
	return out;
}

// remove arcs
void DCClustArcList :: removeArcs() {
	DCClustArcListIter iter(*this);
	DCClustArc* arc;
	DCClustArc* prev = 0;
	
	while ((arc = iter++) != 0) {
		LOG_DEL; delete prev;
		prev = arc;
	}
	LOG_DEL; delete prev;
}
