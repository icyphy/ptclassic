#ifndef _Block_h
#define _Block_h 1

#include "Connect.h"

class Block
{
public:
	// Methods making ports available on the outside;
	// can be read but not set
	int numberPorts() {return ports.size();}
	PortHole& nextPort()  {return ports++;}

	// Get the block name and descriptor
	char* readName() {return name;}
	char* readDescriptor() {return descriptor;}

	// return the full name in the form:
	//   universeName.galaxyName...galaxyName.blockName
	char* readFullName();

	// Pointer to the parent
	Block* blockIamIn;

	// Method setting internal data in the Block
	// If the parent pointer is not provied, it defaults to NULL
	Block& setBlock(char* s, Block* parent = NULL) {
		name=s;
		blockIamIn = parent;
		return *this;
	}

	// Constructor sets key values to prevent segmentation faults
	// if setBlock is not called, or if certain members such as
	// the descriptor are not set.
	Block() {name = "noName";
		 descriptor = "noDescriptor";
		 blockIamIn = NULL;
	}

	// Method to set the parameters of the block
	virtual int setParameters() {};

	// Method to reply "false" if the block contains component
	// blocks that can be seen from the outside.
	// i.e., it is true for stars and wormholes, false for galaxies.
	virtual int isItAtomic () {return TRUE;}

	// Method to print out a description of the block
	// Note that this function flattens the profile of a galaxy,
	// which may not always be wanted.  It produces a great deal
	// of data.
	virtual operator char* ();
protected:
	// Database for this block

	// The following are set from within the Block; hence, protected
	char* descriptor;
	PortList ports;
	
	// Add elements to the to the lists
	addPort(PortHole& p) {ports.put(p);}

private:
	// Name is private, because it should be set only by setBlock()
	// and not changed again.
        char* name;
};

#endif
