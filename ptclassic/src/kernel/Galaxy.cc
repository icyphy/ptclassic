#include "Star.h"
#include <stream.h>
#include "Galaxy.h"
#include "Block.h"

	////////////////////////////////////
	// connect
	////////////////////////////////////

// Need to add error checking

Geodesic& Galaxy :: connect (PortHole& source, PortHole& destination) {

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

	return *geo;
}


Geodesic& Galaxy :: connect (PortHole& source, PortHole& destination,
			int numberDelays) {

	// Call the connect method with two arguments
	Geodesic& geo = connect(source,destination);

	// Set the number of delays
	geo.setInitialParticles(numberDelays);

	return geo;
}

// TO BE DONE:  need a third connect method that accepts a Particle
// reference as a fourth parameter giving the value of the initial Particle
// put on the Geodesic.


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
