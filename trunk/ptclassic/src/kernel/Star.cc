#include "Star.h"
#include <String.h>

Star :: operator char* () {
	String out;
	out = "Star: ";
	out += readFullName();
	out += "\n";
	out += "Descriptor: ";
	out += readDescriptor();
	out += "\n";
	out += "Ports in the star:\n";
	for(int i = numberPorts(); i>0; i--)
		out += nextPort().operator char* ();
	return out;
}

// Redefine method setting internal data in the Block
// so that various SDF-specific initilizations can be performed.
// If the parent pointer is not provied, it defaults to NULL
Block& SDFStar :: setBlock(char* s, Block* parent = NULL) {
	// First invoke the generic setBlock
	Block::setBlock(s,parent);

	// Then perform SDF-specific initializations
	repetitions = 0;
	noTimes = 0;

	return *this;
}
