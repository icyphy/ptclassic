#ifndef _connect_h
#define _connect_h 1

#include "DataStruct.h"
#include "Particle.h"
#include "type.h"

/******************************************************************
This file contains definitions relevant to connections.

A connection between Blocks consists of:

	OutPortHole---->Geodesic----->InPortHole
	    ^				  |
	    |				  |
	    -----------Plasma<-------------

This circular path carries a stream of Particles, where each
Particle is an envelope which contains a piece of data.

PortHole: A data member of a Star, it is where the stream of
	Particles enters or leaves a Star

Geodesic: The place where the Particles reside in transit
	between Stars.

Plasma: The place where Particles reside in transit back.
	Particles are re-used as a way to preserve memory.

Particles: Defined in Particle.h
******************************************************************/

class Geodesic;
class Plasma;
class Block;

        //////////////////////////////////////////
        // class PortHole
        //////////////////////////////////////////

// Contains all the facilities of all PortHoles; base class
//  for all PortHoles
class PortHole
{
public:

        // Every PortHole must be initialized with the setPort function
        // This is not done in the constructor because c++ does not
        // allow us to initialize member classes.
        // Arguments are the name and type (see type.h for supported types).
        PortHole& setPort(char* portName,
                          Block* parent,                // parent block pointer
                          dataType type = FLOAT);       // defaults to FLOAT

        // Name is private (below) so that it can't be changed.
        char* readName() { return name;}

        // In order to trace through the topology graph, we need
        // a pointer to the block to which the PortHole belongs.
        Block* blockIamIn;      // The block I am in.
 
        // Indicate the real port (aliases resolved) at the far end
        // of a connection.  Initialized to NULL.
        // This information is redundant with what's in the geodesic,
        // but to access it through the geodesic, it is necessary
        // to know whether the PortHole is an input or an output.
        PortHole* farSidePort;

        // Print a description of the PortHole
        operator char* ();
 
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

	// Operator to return the last Particle input or output
        // At present, argument must be zero 
        Particle& operator % (int);

	// Maintain pointer to Geodesic
	Geodesic* myGeodesic;

protected:
	// Type of data carried by this PortHole
	dataType type;

	// The Particle that is currently being access
	//  with delay 0 by the Star
	Particle* currentParticle;

	// Maintain pointer to the Plasma where we get
	//  our Particles or replace unused Particles
	Plasma* myPlasma;

private:
	char* name;             // Name of this PortHole
};

        //////////////////////////////////////////
        // class PortList
        //////////////////////////////////////////

// Used to store a list of PortHoles in a MultiPortHole
class PortList : SequentialList
{
        friend class Block;
        friend class MultiPortHole;

        // Return size of list
        int size() {return SequentialList::size();}

        // Return next PortHole on list
        PortHole& operator ++ () {return *(PortHole*)next();}

// Make the following public or a rather large number of MultiPortHoles
// have to be made friends.  This is due to another c++ "feature" that
// derived classes do not inherit friendships.
public:           
        // Add PortHole to list
        void put(PortHole& p) {SequentialList::put(&p);}
};

        //////////////////////////////////////////
        // class MultiPortHole
        //////////////////////////////////////////
                  
// A MultiPortHole is a collection of an indeterminate number of
// PortHoles that have no particular ordering.  Because ordering
// is indeterminate, MultiPortHoles should only be used for commutative
// functions.  The only output MultiPortHole that makes any sense,
// therefore, is one that outputs the same data on all connections.
// Examples of reasonable input MultiPortHoles are summing junctions,
// or an input where all data samples are multiplied together.

// This is its own class for two reasons:  (1) The differences between
// it and ordinary PortHoles are substantial, and (2) this makes it
// easier to overload the connect() function of the Galaxy class to
// properly handle these.  The way the connect function works is that
// every connection to a MultiPortHole causes a new PortHole to be
// created and connected.
 
class MultiPortHole
{
public:
 
        // Every MultiPortHole must be initialized with the setPort function
        // Arguments are the name and type (see type.h for supported types).
        MultiPortHole& setPort(char* portName,
                          Block* parent,                // parent block pointer
                          dataType type = FLOAT);       // defaults to FLOAT
 
        // Name is private (below) so that it can't be changed.
        char* readName() { return name;}
 
        // In order to trace through the topology graph, we need
        // a pointer to the block to which the PortHole belongs.
        Block* blockIamIn;      // The block I am in.
 
        // The PortHole supports only one type of particle at a time
        dataType type;
 
        // Print a description of the PortHole
        operator char* ();
 
        // The MultiPortHole can be aliased to another MultiPortHole.
        MultiPortHole* alias;
 
        // Find the eventual MultiPortHole to which this is aliased
        MultiPortHole& realPort() {
                if (alias == NULL) return *this;
                else return alias->realPort();
                }


        // Determine whether the port is an input or output.
        // For class PortHole, it is unspecified, so both
        // functions return FALSE.
        virtual int isItInput () {return FALSE; }
        virtual int isItOutput () {return FALSE; }

        // Return the number of physical port currently allocated
        int numberPorts() {return ports.size();}

        // return the next port in the list      
        PortHole& operator ++ () {return ports++;}

        // Add a new physical port to the MultiPortHole list
        virtual PortHole& newPort();

protected:                           
        // List of ports allocated
        PortList ports;
private: 
        char* name;             // Name of this PortHole
};

/*****************************************************************
SDF: Synchronous Data Flow

This is a common special case that is handled differently
from other cases:

	Each PortHole promises to consume or generate a fixed
	number of Particles each time the Star is invoked.
	This number is stored in the PortHole and can be accessed
	by the SDFScheduler

	The incrementing of time is forced by the SDFScheduler,
	and not by the Star itself. Incrementing time is effected
	by consuming or generating Particles
****************************************************************/

        //////////////////////////////////////////
        // class SDFPortHole
        //////////////////////////////////////////

// Contains all the special features required for
//   synchronous dataflow (SDF)

class SDFPortHole : public PortHole
{
public:
        // The number of Particles consumed
        unsigned numberTokens;
        
        // The setPort function is redefined to take one more optional
        // argument, the number of Particles consumed
        PortHole& setPort(char* portName,
                          Block* parent,
                          dataType type = FLOAT,        // defaults to FLOAT
                          unsigned numTokens = 1);      // defaults to 1

	// Increment time, by reading in or sending out Particles
        // Argument is the number of Particles
        virtual void increment(int numberTimes) {}

	// Services of PortHole that are often used:
	// setPort(dataType d);
	// Particle& operator % (int);
};

	///////////////////////////////////////////
	// class InSDFPort
	//////////////////////////////////////////

class InSDFPort : public SDFPortHole
{
public:
	int isItInput () {return TRUE; }

	void increment(int numberTimes);

        // Services of PortHole that are often used: 
        // setPort(dataType d); 
        // Particle& operator % (int);
};

	////////////////////////////////////////////
	// class OutSDFPort
	////////////////////////////////////////////

class OutSDFPort : public SDFPortHole
{
public:
        int isItOutput () {return TRUE; }

	void increment(int numberTimes);

        // Services of PortHole that are often used: 
        // setPort(dataType d); 
        // Particle& operator % (int);
};

        //////////////////////////////////////////
        // class MultiSDFPort
        //////////////////////////////////////////
 
// Synchronous dataflow MultiPortHole
 
class MultiSDFPort : public MultiPortHole {
public:
        // The number of Particles consumed
        unsigned numberTokens;
 
        // The setPort function is redefined to take one more optional
        // argument, the number of Particles produced
        MultiPortHole& setPort(char* portName,
                          Block* parent,
                          dataType type = FLOAT,        // defaults to FLOAT
                          unsigned numTokens = 1);      // defaults to 1
};

        //////////////////////////////////////////
        // class MultiSDFInPort
        //////////////////////////////////////////
        
// MultiSDFInPort is an SDF input MultiPortHole
 
class MultiSDFInPort : public MultiSDFPort {
public:
        virtual int isItInput () {return TRUE; }
 
        // Add a new physical port to the MultiPortHole list
        virtual PortHole& newPort();
};
 
 
        //////////////////////////////////////////
        // class MultiSDFOutPort
        //////////////////////////////////////////

// MultiSDFOutPort is an SDF output MultiPortHole  

class MultiSDFOutPort : public MultiSDFPort {     
public:
        virtual int isItOutput () {return TRUE; }

        // Add a new physical port to the MultiPortHole list
        virtual PortHole& newPort();
};

/**************************************************************
Non-SDF PortHoles

In this case, the Star controls the consuming and generation
of Particles internally.

In addition, for convenience we allow the Star to consume a
Particle on an input PortHole, and then put it back into the
Geodesic if it doesnt like what it sees!
**************************************************************/

	//////////////////////////////////////////////
	// class InOtherPort
	//////////////////////////////////////////////

class InOtherPort : public PortHole
{
public:
	// Increment time by inputing Particles from Geodesic
	Particle& operator ++ ();

	// Operator to push the last Particle back on the Geodesic
	Particle& operator -- ();

        // Services of PortHole that are often used: 
        // setPort(dataType d); 
        // Particle& operator % (int);
};

	///////////////////////////////////////
	// class OutOtherPort
	///////////////////////////////////////

class OutOtherPort : public PortHole
{
public:
	// Operator to find a new Particle, and output it
	// from the Star
	Particle& operator ++ ();

        // Services of PortHole that are often used: 
        // setPort(dataType d); 
        // Particle& operator % (int);
};

/****************************************************************
Geodesic

Geodesic is where Particles reside in transit between
Blocks. It is a container class, which stores and retreives
Particles. Actually it stores only Particle*'s, so that its
operator is independent of the type of Particle.
*****************************************************************/
 
	///////////////////////////////////////////
	// class Geodesic
	///////////////////////////////////////////

// Currently a type of Stack
class Geodesic : public Stack
{
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
	// Put a Particle into the Geodesic
	void put(Particle* p) {pushBottom(p);}

	// Get a Particle from the Geodesic
	Particle* get();

	//  Push a Particle back into the Geodesic
	void pushBack(Particle* p) {pushTop(p);}

	// Return the number of Particles on the Geodesic
	int size() {return Stack::size();}

private:
        // For now, the number of Particles on the Geodesic is simply
        // stored as an integer.  Note that the SDFScheduler manipulates
        // this number directly without actually
        // putting any Particles on the geodesic.  The SDFScheduler
        // guarantees that when it is done, the noParticles value will
        // be the same as when it started, or the run will have been aborted
        // due to a sample-rate inconsistency.
        int noParticles;
 
        // Make the SDFScheduler a friend so that noParticles can be
        // manipulated directly.  Note that if the implementation of
        // noParticles changes, so must the implementation of the SDFScheduler
        friend class SDFScheduler;
};

/***********************************************************
Plasma and Particle

See Particle.h for a definition of the Plasma and Particle
************************************************************/

#endif
