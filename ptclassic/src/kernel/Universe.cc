#include "Universe.h"
#include "StringList.h"


// SCCS version identification
// $Id$

SDFUniverse :: operator StringList () {
	StringList out;
	out = "SYNCHRONOUS DATAFLOW UNIVERSE: ";
	out += readFullName ();
	out += "\n";
	out += "Descriptor: ";
	out += readDescriptor();
	out += "\n";
	out += "CONTENTS:\n";

	out += StringList(*myTopology);

	return out;
}
