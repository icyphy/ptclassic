#ifndef _Galaxy_h
#define _Galaxy_h 1

#include "type.h"
#include "Connect.h"
#include "Block.h"
#include "Star.h"

	////////////////////////////////////
	// class BlockList
	////////////////////////////////////

// This class is used to store a list of component blocks in a Galaxy

class BlockList : SequentialList
{
	friend class Galaxy;

	// Add Block to list
	void put(Block* b) {SequentialList::put(b);}

	// Return size of list
	int size() {return SequentialList::size();}

	// Return next Block on list
	Block& operator ++ () {return *(Block*) next();}
};

	////////////////////////////////////
	// class Galaxy
	////////////////////////////////////

class Galaxy : public Block  {

private:
	// Keep a list of component Blocks
	BlockList	blocks;

protected:

	// Add blocks to the list
	void addBlock(Block& b) {blocks.put(&b);}

	// Connect sub-blocks with a delay (default to zero delay)
	// Returns a reference to the Geodesic it created.
	Geodesic& connect(PortHole& source, PortHole& destination,
			  int numberDelays = 0);

	// TO BE DONE:
	// Allow delays to be initialized to something reasonable.
	// Geodesic& connect(PortHole& source, PortHole& destination,
			// int numberDelays, Particle& delayValue);

	// connect is overloaded so that it can accept MultiPortHoles on
	// either input.  Each such call to connect allocates a new PortHole,
	// so in an interactive execution we have to be careful about
	// disconnecting these connections.  We cannot simply remake a
	// connection with another call to connect.
	Geodesic& connect(MultiPortHole& source, PortHole& destination,
			  int numberDelays = 0) {
		return connect(source.realPort().newPort(),
			       destination,
			       numberDelays);
	}
	Geodesic& connect(PortHole& source, MultiPortHole& destination,
			  int numberDelays = 0) {
		return connect(source,
			       destination.realPort().newPort(),
			       numberDelays);
	}
	Geodesic& connect(MultiPortHole& source, MultiPortHole& destination,
			  int numberDelays = 0) {
		return connect(source.realPort().newPort(),
			       destination.realPort().newPort(),
			       numberDelays);
	}


	// Connect a Galaxy PortHole to a PortHole of a sub-block
	alias(PortHole& galPort, PortHole& blockPort) {
		galPort.alias = &blockPort;
	}

	// Overload to alias MultiPortHoles
	alias(MultiPortHole& galPort, MultiPortHole& blockPort) {
		galPort.alias = &blockPort;
	}

public:
	// Return the number of blocks in the galaxy.
	int numberBlocks() {return blocks.size();}

	// Return the next block in the list
	Block& nextBlock() {return blocks++;}

	// Print a description of the galaxy
	virtual operator char* ();

	// Method replies FALSE to indicate that component blocks
	// can be seen from outside.
	virtual int isItAtomic () {return FALSE;}
};


#endif
