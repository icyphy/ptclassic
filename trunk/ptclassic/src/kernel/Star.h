#ifndef _Star_h
#define _Star_h 1

#include "type.h"
#include "Connect.h"
#include "Block.h"

	////////////////////////////////////
	// class Star
	////////////////////////////////////

class Star : public Block  {

// Have to make the following public or make every individual
// scheduler a friend.  It is not sufficient to make the base class
// Scheduler a friend (is this yet another language defect?).

public:
	// The writer of a Star code has the option to
	// provide the following methods

	// Access the parameters by name, and store them
	// in the associated parameter variables
	// This function is virtual in base class Block, so put
	// in the Star only if needed
	setParameters() {};

	// Initialize the simulation; i.e. do everything necessary
	// to start over, like initializing state variables
        virtual int initialize() {};

	// Run the simulation, accessing Particles on the
	// input PortHoles, generating Particles on the output PortHoles
        virtual int go() {}; 

	// Perform functions at wrapup, such as collecting stats
        virtual int wrapup() {};

	// Method to print out description
	virtual void profile ();
};

/*
class OutputToUser is for purpose of output from a Star.....
we want to avoid direct output, since Cosmos should maintain
control over where output goes (wherever in the Universe
the user interface may be!)
*/

class OutputToUser
{
	// Rather than using ostream pointer, which would require
	// recompiling stream.h for each Star, we use an Pointer
	Pointer outputStream;

	void outputString(char*);
public:
	OutputToUser();
	void fileName		(char *);	// Name of file for output
	void operator <<	 (int);		// Output data to the user
	void operator <<	 (char*);
	void operator <<	 (float);
};

#endif
