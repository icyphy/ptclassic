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
