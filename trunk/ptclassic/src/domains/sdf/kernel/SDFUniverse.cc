#include "Universe.h"
#include "StringList.h"

SDFUniverse :: operator char* () {
	StringList out;
	out = "SYNCHRONOUS DATAFLOW UNIVERSE: ";
	out += readFullName ();
	out += "\n";
	out += "Descriptor: ";
	out += readDescriptor();
	out += "\n";
	out += "CONTENTS:\n";

	out += *myTopology;

	return out;
}
