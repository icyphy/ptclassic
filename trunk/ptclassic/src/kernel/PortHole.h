#ifndef _connect_h
#define _connect_h 1

#include "type.h"
#include "SeqList.h"

class Geodesic;
class Block;

	//////////////////////////////////////////
	// class PortHole
	//////////////////////////////////////////

class PortHole
{
public:

	// Every PortHole must be initialized with the setPort function
	// This is not done in the constructor because c++ does not
	// allow us to initialize member classes.
	// Arguments are the name and type (see type.h for supported types).
	PortHole& setPort(char* portName,
			  Block* parent,		// parent block pointer
		          dataType type = FLOAT);	// defaults to FLOAT

	// Name is private (below) so that it can't be changed.
	char* readName() { return name;}

	Geodesic* myGeodesic;	// Geodesic connected to this PortHole

	// In order to trace through the topology graph, we need
	// a pointer to the block to which the PortHole belongs.
	Block* blockIamIn;	// The block I am in.

	// Indicate the real port (aliases resolved) at the far end
	// of a connection.  Initialized to NULL.
	// This information is redundant with what's in the geodesic,
	// but to access it through the geodesic, it is necessary
	// to know whether the PortHole is an input or an output.
	PortHole* farSidePort;

	// The PortHole supports only one type of particle at a time
	dataType type;

	// Print a description of the PortHole
	void profile();

	// The PortHole can be aliased to another PortHole.
	// For example, any PortHole belonging to a Galaxy is aliased
	// to a PortHole belonging to a Star in the Galaxy.
	PortHole* alias;

	// Find the eventual PortHole to which this is aliased
	PortHole& realPort() {
		if (alias == NULL) return *this;
		else return alias->realPort();
	}

	// Determine whether the port is an input or output.
	// For class PortHole, it is unspecified, so both
	// functions return FALSE.
	// These functions are necessary to be able to check
	// the validity of a connection.
	virtual int isItInput () {return FALSE; }
	virtual int isItOutput () {return FALSE; }
	
private:
	char* name;		// Name of this PortHole
};


	//////////////////////////////////////////
	// class SDFPortHole
	//////////////////////////////////////////

// Synchronous dataflow PortHole

class SDFPortHole : public PortHole {
public:
	// The number of tokens consumed
	unsigned numberTokens;

	// The setPort function is redefined to take one more optional
	// argument, the number of tokens produced.
	PortHole& setPort(char* portName,
			  Block* parent,
		          dataType type = FLOAT,	// defaults to FLOAT
		          unsigned numTokens = 1);	// defaults to 1
};


	//////////////////////////////////////////
	// class InPortHole
	//////////////////////////////////////////

// InPortHole is an SDF InPortHole.

class InPortHole : public SDFPortHole {
public:
	virtual int isItInput () {return TRUE; }
};


	//////////////////////////////////////////
	// class OutPortHole
	//////////////////////////////////////////

// OutPortHole is an SDF OutPortHole.

class OutPortHole : public SDFPortHole {
public:
	virtual int isItOutput () {return TRUE; }
};


	//////////////////////////////////////////
	// class PortList
	//////////////////////////////////////////


class PortList : SequentialList
{
        friend class Block;

        // Add PortHole to list
        void put(PortHole& p) {SequentialList::put(&p);}
 
        // Return size of list
        int size() {return SequentialList::size();}
 
        // Return next PortHole on list
        PortHole& operator ++ () {return *(PortHole*)next();}
};


	//////////////////////////////////////////
	// class Geodesic
	//////////////////////////////////////////

class Geodesic
{
private:
	// For now, the number of Particles on the Geodesic is simply
	// stored as an integer.  Note that the SDFScheduler manipulates
	// this number using the public methods below without actually
	// putting any Particles on the geodesic.  The SDFScheduler
	// guarantees that when it is done, the noParticles value will
	// be the same as when it started, or the run will have been aborted
	// due to a sample-rate inconsistency.
	int noParticles;

	// Make the SDFScheduler a friend so that noParticles can be
	// manipulated directly.  Note that if the implementation of
	// noParticles changes, so must the implementation of the SDFScheduler
	friend class SDFScheduler;
public:
        // We keep a pointer to the PortHoles corresponding to
        // this Geodesic
        PortHole *originatingPort;
        PortHole *destinationPort;

	// Constructor
	Geodesic() { originatingPort = NULL;
		     destinationPort = NULL;
		     noParticles = 0;}

	// Return the number of Particles currently on the Geodesic
	int getNoParticles() {return noParticles;}

	// When a connection contains a delay, initial particles
	// should be put on the Geodesic.  Something should be added
	// here to put the right number of zero-valued particles
	// on the geodesic (what is meant by "zero-valued" will
	// depend on the Particle type).  Also, setInitialParticles
	// should optionally accept two arguments,
	// the second of which is a Particle& specifying
	// some specific initial value.
	void setInitialParticles (int howMany) {noParticles = howMany;}
};

#endif
