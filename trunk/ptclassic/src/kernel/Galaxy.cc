#include "Star.h"
#include <stream.h>
#include "Galaxy.h"
#include "Block.h"

	////////////////////////////////////
	// connect
	////////////////////////////////////

// Need to add error checking

void Galaxy :: connect (PortHole& source, PortHole& destination) {

	// Resolve any aliases
	PortHole& realSource = source.realPort();
	PortHole& realDest = destination.realPort();

	Geodesic* geo = new Geodesic;
	geo->originatingPort = &realSource;
	geo->destinationPort = &realDest;
	realSource.myGeodesic = geo;
	realDest.myGeodesic = geo;

	// Set the farSidePort pointers in both blocks
	// This information is redundant, since it also appears in the
	// Geodesic, but to get it from the Geodesic, you have to know
	// which PortHole is an input, and which is an output.
	realSource.farSidePort = &realDest;
	realDest.farSidePort = &realSource;
}

	////////////////////////////////////
	// profile
	////////////////////////////////////

// Profile function will later be replaced with operator (char*).

void Galaxy :: profile() {
	cout << "GALAXY: " << readFullName() << "\n";
	cout << "Descriptor: " << readDescriptor() << "\n";
	cout << "Number of blocks: " << numberBlocks() << "\n";
	if (numberPorts()>0) cout << "Ports in the Galaxy:\n";
	for(int i = numberPorts(); i>0; i--)
		nextPort().profile();

	cout << "Blocks in the Galaxy:----------------------------------\n";
	for(i = numberBlocks(); i>0; i--) {
		nextBlock().profile();
	}
}
