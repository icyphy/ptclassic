// SCCS version identification
// $Id$
// Joseph T. Buck
//
// Print out the universe

#include "Universe.h"
#include "StringList.h"

StringList
Universe :: print (int recursive) {
	StringList out;
	out = type;
	out += " UNIVERSE: ";
	out += readFullName ();
	out += "\n";
	out += "Descriptor: ";
	out += readDescriptor();
	out += "\n";
	out += "CONTENTS:\n";

	if (recursive)
		out += Galaxy::printRecursive();
	else
		out += Galaxy::printVerbose();
	return out;
}

// complete the simulation
void Runnable::wrapupGal (Galaxy& g) {
	g.wrapup();
	for (int i=g.numberBlocks(); i>0; i--) {
		Block& b = g.nextBlock();
		if (b.isItAtomic()) b.wrapup();
		else wrapupGal (b.asGalaxy());
	}
}
	
