#include "SDFUniverse.h"
#include "StringList.h"


// SCCS version identification
// $Id$

StringList
SDFUniverse :: printVerbose () {
	StringList out;
	out = "SYNCHRONOUS DATAFLOW UNIVERSE: ";
	out += readFullName ();
	out += "\n";
	out += "Descriptor: ";
	out += readDescriptor();
	out += "\n";
	out += "CONTENTS:\n";

	out += myTopology->printVerbose();

	return out;
}
