#include "Universe.h"
#include <stream.h>

// TO BE DONE: Make this a cast to char*
void SDFUniverse :: profile () {
	cout << "SYNCHRONOUS DATAFLOW UNIVERSE: " << readFullName () << "\n";
	cout << "Descriptor: " << readDescriptor() << "\n";
	cout << "CONTENTS:" << "\n";

	myTopology->profile();
}
