#include "Universe.h"
#include <String.h>

SDFUniverse :: operator char* () {
	String out;
	out = "SYNCHRONOUS DATAFLOW UNIVERSE: ";
	out += readFullName ();
	out += "\n";
	out += "Descriptor: ";
	out += readDescriptor();
	out += "\n";
	out += "CONTENTS:";
	out += "\n";

	out += *myTopology;

	return out;
}
