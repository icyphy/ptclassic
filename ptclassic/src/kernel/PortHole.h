#ifndef _connect_h
#define _connect_h 1

#include "NamedObj.h"
#include "DataStruct.h"
#include "Particle.h"
#include "type.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 1/17/89
 Revisions:
 	3/19/90 - J. Buck
		Make MultiPortHole a derived type of PortHole.
		add a virtual method newConnection to get the
		PortHole to use for a new connection (realPort
		for PortHoles, realPort.newPort for MultiPortHoles).
		Allow casting from MultiPortHole to PortHole.

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

	/////////////////////////////////////////
	// class CircularBuffer
	/////////////////////////////////////////

/*
class CircularBuffer manages a circular buffer
(array with modulo addressing) containing Pointer's

It is used to store Particle*'s corresponding to past
input or output Particles
*/

class CircularBuffer
{
public:
        // Argument is the dimension of the array to allocate
        CircularBuffer(int);
        ~CircularBuffer();

        // Reset to the beginning of the buffer
        void reset() {current=0;}

	// ZERO out the contents of the buffer
	void initialize();

        // Return current Pointer on the buffer
        Pointer* here();

        // Return next Pointer on the buffer
        Pointer* next();

        // Back up one Pointer on the buffer
        Pointer* last();
 
        // Access buffer relative to current
        Pointer* previous(int);
 
        // Size of the buffer
        int size() const {return dimen;}
 
private:
        // Number of Pointers on the buffer
        int dimen;
        // Index of the current Pointer
        int current;
        // Pointer array
        Pointer* buffer;
};

class Geodesic;
class Plasma;
class Block;

	//////////////////////////////////////////
	// class GenericPort
	//////////////////////////////////////////

// Base class for PortHole and MultiPortHole; it is derived
// from NamedObj.

class PortHole;

class GenericPort : public NamedObj
{
public:
        // Determine whether the port is an input or output.
        // For class PortHole, it is unspecified, so both
        // functions return FALSE.
        virtual int isItInput () {return FALSE; }
        virtual int isItOutput () {return FALSE; }

	// print info on the PortHole
	operator StringList ();

	// virtual function used for new connections.
	// PortHole uses this one unchanged; MultiPortHole has to create
	// a new Port.
	virtual PortHole& newConnection() {
	// my apologies for this horrible cast
		return *(PortHole *)&GenericPort::realPort();
	}

	GenericPort& realPort();

	GenericPort& setPort(const char* portName, Block* blk, dataType typ=FLOAT) {
		setNameParent (portName, blk);
		type = typ;
		alias = NULL;
		return *this;
	}
protected:
	// datatype of particles in this porthole
	dataType type;

	// PortHole this is aliased to
	GenericPort* alias;
};

	
        //////////////////////////////////////////
        // class PortHole
        //////////////////////////////////////////

// Contains all the facilities of all PortHoles; base class
//  for all PortHoles
class PortHole : public GenericPort
{
public:

        // Every PortHole must be initialized with the setPort function
        // This is not done in the constructor because c++ does not
        // allow us to initialize member classes.
        // Arguments are the name and type (see type.h for supported types).
        PortHole& setPort(const char* portName,
                          Block* parent,                // parent block pointer
                          dataType type = FLOAT);       // defaults to FLOAT

	// Initialize when starting a simulation
	void initialize();

        // Indicate the real port (aliases resolved) at the far end
        // of a connection.  Initialized to NULL.
        // This information is redundant with what's in the geodesic,
        // but to access it through the geodesic, it is necessary
        // to know whether the PortHole is an input or an output.
        PortHole* farSidePort;

        // Print a description of the PortHole
        operator StringList ();

	// set the alias
	setAlias (PortHole& blockPort) { alias = &blockPort; }

	// Methods that are called by the Star::beforeGo()
	//  and Star::afterGo() before and after go()
	// Can be used for things like inputing and output
	//  Particles
	virtual void beforeGo() {}
	virtual void afterGo() {}

	// Operator to return Particles previously input or output
	// Argument is the delay in the past
        Particle& operator % (int);

	// Set the maximum delay that past Particles can be
	//  accessed -- defaults to zero if never called
	void setMaxDelay(int delay);

	// Number of Particles stored in the buffer each
	// time the Geodesic is accessed -- normally this is
	// one except for SDF, where it is the number of
	// Particles consumed or generated
	int numberTokens;

	// Maintain pointer to Geodesic connected to this PortHole
	Geodesic* myGeodesic;

	// Maintain pointer to the Plasma where we get
	//  our Particles or replace unused Particles
	Plasma* myPlasma;

	// Allocate a return a Geodesic compatible with this
	// type of PortHole
	virtual Geodesic* allocateGeodesic();

protected:
	// Buffer where the Particle*'s are stored
	CircularBuffer* myBuffer;

	// Allocate new buffer
	void allocateBuffer(int size);
};

        //////////////////////////////////////////
        // class PortList
        //////////////////////////////////////////

// Used to store a list of PortHoles in a MultiPortHole
class PortList : SequentialList
{
        friend class Block;
        friend class MultiPortHole;

	void initialize() {SequentialList::initialize();}

        // Return size of list
        int size() const {return SequentialList::size();}

	// Reset the list to beginning
	void reset() {SequentialList::reset();}

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

class MultiPortHole: public GenericPort
{
public:
	void initialize() {ports.initialize();}
 
        // Every MultiPortHole must be initialized with the setPort function
        // Arguments are the name and type (see type.h for supported types).
        MultiPortHole& setPort(const char* portName,
                          Block* parent,                // parent block pointer
                          dataType type = FLOAT);       // defaults to FLOAT
 
        // Return the number of physical port currently allocated
        int numberPorts() {return ports.size();}

        // Return the next port in the list      
        PortHole& operator () () {return ports++;}

	// Reset the list to the beginning
	void reset() {ports.reset();}

        // Add a new physical port to the MultiPortHole list
        virtual PortHole& newPort();

	MultiPortHole& realPort() {
	// my apologies for this horrible cast.  It is safe because
	// alias for a MultiPortHole is always a MultiPortHole.
		return *(MultiPortHole *)&GenericPort::realPort();
	}

	// set alias for MultiPortHole
	setAlias (MultiPortHole &blockPort) { alias = &blockPort;}

	// Return a new port for connections
	virtual PortHole& newConnection() { return realPort().newPort();}

	// Also use this in casting to PortHole.  (Is this what we want?)
	operator PortHole (){ return newConnection();}

        // Print a description of the MultiPortHole
        virtual operator StringList ();

protected:                           
        // List of ports allocated
        PortList ports;
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
        // The setPort function is redefined to take one more optional
        // argument, the number of Particles consumed/generated
        PortHole& setPort(const char* portName,
                          Block* parent,
                          dataType type = FLOAT,
			  // Number Particles consumed/generated
                          unsigned numTokens = 1,
			  // Maximum delay the Particles are accessed
			  unsigned delay = 0);

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

	// Get Particles from input Geodesic
	void beforeGo();

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

	void increment();

	// Move the current Particle in the input buffer -- this
	// method is invoked by the SDFScheduler before go()
	void beforeGo();

	// Put the Particles that were generated into the
	// output Geodesic -- this method is invoked by the
	// SDFScheduler after go()
	void afterGo();

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
        MultiPortHole& setPort(const char* portName,
                          Block* parent,
                          dataType type = FLOAT,        // defaults to FLOAT
                          unsigned numTokens = 1);      // defaults to 1
};

        //////////////////////////////////////////
        // class MultiInSDFPort
        //////////////////////////////////////////
        
// MultiInSDFPort is an SDF input MultiPortHole
 
class MultiInSDFPort : public MultiSDFPort {
public:
        int isItInput () {return TRUE; }
 
        // Add a new physical port to the MultiPortHole list
        PortHole& newPort();
};
 
 
        //////////////////////////////////////////
        // class MultiOutSDFPort
        //////////////////////////////////////////

// MultiOutSDFPort is an SDF output MultiPortHole  

class MultiOutSDFPort : public MultiSDFPort {     
public:
        int isItOutput () {return TRUE; }

        // Add a new physical port to the MultiPortHole list
        PortHole& newPort();
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
	// A lot of things are kept public, unfortunately, because
	//  these routines are called by a lot of different
	//  types of PortHoles -- impossible to make them
	//  all friends

        // We keep a pointer to the PortHoles corresponding to
        // this Geodesic
        PortHole *originatingPort;
        PortHole *destinationPort;

        // Constructor
        Geodesic() { originatingPort = NULL;
                     destinationPort = NULL;
		     numInitialParticles = 0;}

	// initialize() for Geodesic initializes the number of Particles
	// to that given by the numInitialParticles field, and
	// also calls initialize() for each of those Particles
	// TO BE DONE:  There should be a way to specify the value
	// of these initial particles.
	void initialize();

	// Put a Particle into the Geodesic
	void put(Particle* p) {pushBottom(p);}

	// Get a Particle from the Geodesic
	Particle* get();

	//  Push a Particle back into the Geodesic
	void pushBack(Particle* p) {pushTop(p);}

	// Return the number of Particles on the Geodesic
	int size() {return Stack::size();}

        // A connection may require some initial particles.
        // Note that the SDFScheduler manipulates this number directly, but
        // guarantees that when it is done, the value will
        // be the same as when it started, or the run will have been aborted
        // due to a sample-rate inconsistency.
	int numInitialParticles;
};

/***********************************************************
Plasma and Particle

See Particle.h for a definition of the Plasma and Particle
************************************************************/

#endif
