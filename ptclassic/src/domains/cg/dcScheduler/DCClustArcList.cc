static const char file_id[] = "DCClustArcList.cc";
/*****************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
			All Rights Reserved.

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
