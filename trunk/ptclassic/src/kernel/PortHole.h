#ifndef _connect_h
#define _connect_h 1

#include "NamedObj.h"
#include "DataStruct.h"
#include "dataType.h"
#include "Particle.h"
#include "type.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 1/17/90
 Revisions:
 	3/19/90 - J. Buck
		Make MultiPortHole a derived type of PortHole.
		add a virtual method newConnection to get the
		PortHole to use for a new connection (realPort
		for PortHoles, realPort.newPort for MultiPortHoles).
		Allow casting from MultiPortHole to PortHole.

	5/29/90 - J. Buck
		Move SDF-specific portholes to SDFConnect.h

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
class CircularBuffer;
class Geodesic;
class Plasma;
class Block;
class Galaxy;

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
        virtual int isItInput () const;
        virtual int isItOutput () const;

	// Determine whether the port is a multiporthole.
	virtual int isItMulti () const;

	// print info on the PortHole
	StringList printVerbose () const;

	// virtual function used for new connections.
	// PortHole uses this one unchanged; MultiPortHole has to create
	// a new Port.
	virtual PortHole& newConnection();

	// Translate aliases, if any.
	GenericPort& realPort() const {
		const GenericPort* p = this;
		while (p->aliasedTo) p = p->aliasedTo;
		return *p;
	}

	GenericPort& setPort(const char* portName, Block* blk, dataType typ=FLOAT) {
		setNameParent (portName, blk);
		type = typ;
		return *this;
	}

	// set up a port for determining the type of ANYTYPE connections.
	// typePort pointers form a circular loop.
	void inheritTypeFrom(GenericPort& p);

	// function to initialize PortHole Plasmas
	virtual Plasma* setPlasma(Plasma *useType = NULL) = 0;


	// function to connect two portholes
	virtual void connect(GenericPort& destination,int numberDelays);

	// return my type
	dataType myType () const { return type;}

	GenericPort* alias() const { return aliasedTo;}

	// Constructor
	GenericPort () : type(ANYTYPE),aliasedTo(0),typePortPtr(0),aliasedFrom(0) {}

	// Destructor
	~GenericPort();
	
protected:
	// set up alias pointers in pairs.  Protected so derived types may
	// restrict who can be an alias.
	void setAlias (GenericPort& gp) {
		aliasedTo = &gp;
		gp.aliasedFrom = this;
	}

	// datatype of particles in this porthole
	dataType type;

	// return typePortPtr
	GenericPort* typePort() const { return typePortPtr;}

private:
	// PortHole this is aliased to
	GenericPort* aliasedTo;

	// Name of a PortHole that is aliased to me (a back-pointer)
	GenericPort* aliasedFrom;

	// My type matches type of this port
	GenericPort* typePortPtr;
};


        //////////////////////////////////////////
        // class PortHole
        //////////////////////////////////////////

// Contains all the facilities of all PortHoles; base class
//  for all PortHoles
class PortHole : public GenericPort
{
	friend class Geodesic;	// allow Geodesic to access myPlasma
	friend setPortIndices(Galaxy&);
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

	// Remove a connection, and optionally attempt to delete
	// the geodesic.  Arg is set to zero only when called from
	// the geodesic's destructor.  We do not delete "persistent"
	// geodesics.
	virtual void disconnect(int delGeo = 1);

	// Return the porthole we are connected to (see below)
	PortHole* far() const { return farSidePort;}

        // Print a description of the PortHole
	StringList printVerbose () const;

	// set the alias
	void setAlias (PortHole& blockPort) {
		GenericPort::setAlias (blockPort);
	}

	// Can be used for things like inputing and output
	//  Particles.  These are currently do-nothing functions
	virtual void grabData(); 
	virtual void sendData();

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

	// return the number of particles on my Geodesic
	int numTokens() const;

	// Maintain pointer to Geodesic connected to this PortHole
	Geodesic* myGeodesic;

	// Allocate a return a Geodesic compatible with this
	// type of PortHole
	virtual Geodesic* allocateGeodesic();

	// initialize the Plasma
	Plasma* setPlasma(Plasma *useType = NULL);

	// Constructor: just worry about pointers here
	PortHole () : myGeodesic(0), farSidePort(0), myPlasma(0),
		      myBuffer(0) {}
	
	// Destructor
	~PortHole ();

	// index value
	int index() const { return indexValue;}
protected:
        // Indicate the real port (aliases resolved) at the far end
        // of a connection.  Initialized to NULL.
        // This information is redundant with what's in the geodesic,
        // but to access it through the geodesic, it is necessary
        // to know whether the PortHole is an input or an output.
        PortHole* farSidePort;

	// Maintain pointer to the Plasma where we get
	//  our Particles or replace unused Particles
	Plasma* myPlasma;

	// Buffer where the Particle*'s are stored
	CircularBuffer* myBuffer;

	// size of buffer to allocate
	int bufferSize;

	// get Particle from the Geodesic
	void getParticle();

	// put Particle to the Geodesic
	void putParticle();

	// clear Particle
	void clearParticle();
private:
	// Allocate new buffer
	void allocateBuffer();

	// index value, for making scheduler tables
	int indexValue;
};

// The following generic types are good enough to use in galaxies.
// They are just like PortHole except that they indicate whether
// they are input or output.

        //////////////////////////////////////////
        // class InPortHole
        //////////////////////////////////////////

class InPortHole : public PortHole {
public:
	int isItInput() const; // return TRUE
};

        //////////////////////////////////////////
        // class OutPortHole
        //////////////////////////////////////////

class OutPortHole : public PortHole {
public:
	int isItOutput() const; // {return TRUE;}
};

        //////////////////////////////////////////
        // class PortList
        //////////////////////////////////////////

// Used to store a list of PortHoles in a MultiPortHole
class PortList : public SequentialList
{
public:

        // Add PortHole to list
        void put(PortHole& p) {SequentialList::put(&p);}
};

        //////////////////////////////////////////
        // class PortListIter
        //////////////////////////////////////////

// An iterator for PortLists
class PortListIter : private ListIter {
public:
	PortListIter(const PortList& plist) : ListIter (plist) {}
	PortHole* next() { return (PortHole*)ListIter::next();}
	PortHole* operator++() { return next();}
	ListIter::reset;
};


        //////////////////////////////////////////
        // class MultiPortHole
        //////////////////////////////////////////
                  
// A MultiPortHole is a collection of an indeterminate number of
// PortHoles.

class MultiPortHole: public GenericPort
{
	friend class MPHIter;
public:
	void initialize();

	// virtual function to identify multi-ness
	int isItMulti() const; // {return TRUE;}

        // Every MultiPortHole must be initialized with the setPort function
        // Arguments are the name and type (see type.h for supported types).
        MultiPortHole& setPort(const char* portName,
                          Block* parent,                // parent block pointer
                          dataType type = FLOAT);       // defaults to FLOAT
 
        // Return the number of physical port currently allocated
        int numberPorts() const {return ports.size();}

        // Add a new physical port to the MultiPortHole list
        virtual PortHole& newPort();

	MultiPortHole& realPort() {
	// my apologies for this horrible cast.  It is safe because
	// alias for a MultiPortHole is always a MultiPortHole.
		return *(MultiPortHole *)&GenericPort::realPort();
	}

	// set alias for MultiPortHole
	void setAlias (MultiPortHole &blockPort) {
		GenericPort::setAlias (blockPort);
	}

	// Return a new port for connections
	virtual PortHole& newConnection();

	// Also use this in casting to PortHole.  (Is this what we want?)
	operator PortHole (){ return newConnection();}

        // Print a description of the MultiPortHole
	StringList printVerbose () const;

	// function to set Plasma type of subportholes
	Plasma* setPlasma(Plasma *useType = NULL);

	// destructor
	~MultiPortHole();
protected:                           
        // List of ports allocated
        PortList ports;

	// Method for generating names for contained PortHoles
	char* newName();

	// add a newly created port to the multiporthole
	PortHole& installPort(PortHole& p);
};

        //////////////////////////////////////////
        // class MPHList
        //////////////////////////////////////////

// Used to store a list of MultiPortHoles
class MPHList : public SequentialList
{
public:
        // Add MultiPortHole to list
        void put(MultiPortHole& p) {SequentialList::put(&p);}
};

        //////////////////////////////////////////
        // class MPHIter
        //////////////////////////////////////////

// Iterator for MultiPortHoles
class MPHIter : public PortListIter {
public:
	MPHIter(const MultiPortHole& mph) : PortListIter (mph.ports) {}
};

/***********************************************************
Plasma and Particle

See Particle.h for a definition of the Plasma and Particle
************************************************************/

#endif
