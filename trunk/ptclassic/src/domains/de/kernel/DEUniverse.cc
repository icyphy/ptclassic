// Methods for DEUniverse
// Soonhoi Ha
// $Id$

#include "DEUniverse.h"
#include "StringList.h"

StringList
DEUniverse :: printVerbose () {
	StringList out;
	out = "DISCRETE EVENT UNIVERSE: ";
	out += readFullName ();
	out += "\n";
	out += "Descriptor: ";
	out += readDescriptor();
	out += "\n";
	out += "CONTENTS:\n";

	out += myTopology->printVerbose();

	return out;
}
