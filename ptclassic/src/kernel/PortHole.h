#ifndef _connect_h
#define _connect_h 1

#ifdef __GNUG__
#pragma once
#pragma interface
#endif

#include "NamedObj.h"
#include "DataStruct.h"
#include "dataType.h"
#include "Particle.h"
#include "Attribute.h"
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

// attribute bits for PortHoles.  Only one is defined here: PB_HIDDEN.
// if it is true, the porthole is not visible to user interfaces, so
// the user will not see it.  It is off by default.

const bitWord PB_HIDDEN = 1;
extern const Attribute P_HIDDEN; // make the porthole hidden
extern const Attribute P_VISIBLE;// make the porthole visible.

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

	GenericPort& realPort() {
		return *translateAliases();
	}

	// this one permits use in const expressions
	const GenericPort& realPort() const;

	GenericPort& setPort(const char* portName, Block* blk, DataType typ=FLOAT) {
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
	DataType myType () const { return type;}

	// class identification
	int isA(const char*) const;

	GenericPort* alias() const { return aliasedTo;}

	// Constructor
	GenericPort ();

	// Destructor
	~GenericPort();
	
	// attributes
	bitWord attributes() const { return attributeBits;}

	bitWord setAttributes(const Attribute& attr) {
		return attributeBits = attr.eval(attributeBits);
	}

	// return typePortPtr
	GenericPort* typePort() const { return typePortPtr;}

protected:
	// set up alias pointers in pairs.  Protected so derived types may
	// restrict who can be an alias.
	void setAlias (GenericPort& gp) {
		aliasedTo = &gp;
		gp.aliasedFrom = this;
	}

	// Translate aliases, if any.
	GenericPort* translateAliases();

	// datatype of particles in this porthole
	DataType type;

private:
	// PortHole this is aliased to
	GenericPort* aliasedTo;

	// Name of a PortHole that is aliased to me (a back-pointer)
	GenericPort* aliasedFrom;

	// My type matches type of this port
	GenericPort* typePortPtr;

	// My attributes
	bitWord attributeBits;

};

// predicate for a hidden porthole (or multiporthole)
inline int hidden(const GenericPort& p) { return p.attributes() & PB_HIDDEN; }


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
                          DataType type = FLOAT);       // defaults to FLOAT

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

	// class identification
	int isA(const char*) const;

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

	// Constructor
	PortHole ();
	
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

// The following generic type is good enough to use in galaxies.
// It always has an alias.
// The "isIt" functions work by asking the alias.

        //////////////////////////////////////////
        // class GalPort
        //////////////////////////////////////////

class GalPort : public PortHole {
public:
	GalPort(GenericPort& a);
	int isItInput() const;
	int isItOutput() const;
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

// An iterator for PortLists, non-const version
class PortListIter : private ListIter {
public:
	PortListIter(PortList& plist) : ListIter (plist) {}
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
	friend class CMPHIter;
public:
	void initialize();

	// virtual function to identify multi-ness
	int isItMulti() const; // {return TRUE;}

	// class identification
	int isA(const char*) const;

        // Every MultiPortHole must be initialized with the setPort function
        // Arguments are the name and type (see type.h for supported types).
        MultiPortHole& setPort(const char* portName,
                          Block* parent,                // parent block pointer
                          DataType type = FLOAT);       // defaults to FLOAT
 
        // Return the number of physical port currently allocated
        int numberPorts() const {return ports.size();}

        // Add a new physical port to the MultiPortHole list
        virtual PortHole& newPort();

	MultiPortHole& realPort() {
	// my apologies for this horrible cast.  It is safe because
	// alias for a MultiPortHole is always a MultiPortHole.
		return *(MultiPortHole *)translateAliases();
	}

	// set alias for MultiPortHole
	void setAlias (MultiPortHole &blockPort) {
		GenericPort::setAlias (blockPort);
	}

	// Return a new port for connections
	virtual PortHole& newConnection();

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
	const char* newName();

	// add a newly created port to the multiporthole
	PortHole& installPort(PortHole& p);
};

        //////////////////////////////////////////
        // class GalMultiPort
        //////////////////////////////////////////

class GalMultiPort : public MultiPortHole {
public:
	// a GalMultiPort always has an alias
	GalMultiPort(GenericPort& a);

	// queries pass through to the inside
	int isItInput() const;
	int isItOutput() const;

	// when making a new port, create it both locally and in the
	// alias and connect the two together.
	PortHole& newPort();
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
	MPHIter(MultiPortHole& mph) : PortListIter (mph.ports) {}
};

#endif
