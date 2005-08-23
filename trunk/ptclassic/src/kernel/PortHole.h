#ifndef _PortHole_h
#define _PortHole_h 1

#ifdef __GNUG__
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
@(#)PortHole.h	2.48	12/16/98

Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

 Programmer:  E. A. Lee and D. G. Messerschmitt, J. Buck
 Date of creation: 1/17/90

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
	Particles are re-used as a way to conserve memory.

By default, all connections with the same resolved DataType use
the same global Plasma.  This can be changed by overriding
the allocatePlasma function.  Replacing it with a call to
allocateLocalPlasma will use local Plasmas for each connection.

Particles: Defined in Particle.h

A MultiPortHole is a set of related portholes.

******************************************************************/
class CircularBuffer;
class Geodesic;
class Plasma;
class Block;
class Galaxy;
class EventHorizon;
class ToEventHorizon;
class FromEventHorizon;
class PtGate;

	//////////////////////////////////////////
	// class GenericPort
	//////////////////////////////////////////

// Base class for PortHole and MultiPortHole; it is derived
// from NamedObj.

class PortHole;
class MultiPortHole;
class ClusterPort;

// attribute bits for PortHoles.  Only one is defined here: PB_HIDDEN.
// if it is true, the porthole is not visible to user interfaces, so
// the user will not see it.  It is off by default.

const bitWord PB_HIDDEN = 1;
extern const Attribute P_HIDDEN; // make the porthole hidden
extern const Attribute P_VISIBLE;// make the porthole visible.

class GenericPort : public NamedObj
{
public:
	// Constructor
	GenericPort ();

	// Destructor
	~GenericPort();

        // Determine whether the port is an input or output.
        // For class PortHole, it is unspecified, so both
        // functions return FALSE.
        virtual int isItInput () const;
        virtual int isItOutput () const;

	// Determine whether the port is a multiporthole.
	virtual int isItMulti () const;

	// print info on the PortHole
	/* virtual*/ StringList print (int verbose = 0) const;

	// Initialize porthole properties.
	GenericPort& setPort(const char* portName, Block* blk, DataType typ=FLOAT) {
		setNameParent (portName, blk);
		myType = typ;
		return *this;
	}

	// set up a link for determining the type of ANYTYPE connections.
	// this would be better named sameTypeAs(), since it is a completely
	// symmetric constraint.
	void inheritTypeFrom(GenericPort& p);

	// return typePortPtr
	inline GenericPort* typePort() const { return typePortPtr;}

	// return my declared type
	// caution: declared type may not be the particle type actually in use!
	inline DataType type () const { return myType;}

	// class identification
	int isA(const char*) const;
	
	// attributes
	inline bitWord attributes() const { return attributeBits;}

	inline bitWord setAttributes(const Attribute& attr) {
		return attributeBits = attr.eval(attributeBits);
	}

	// function to connect two portholes
	// 3/2/94 changed to support initDelayValues
	virtual void connect(GenericPort& destination,int numberDelays,
			     const char* initDelayValues = 0);

	// Get a porthole suitable for connecting to (never an alias).
	// PortHole just returns its realPort (bottom alias);
	// MultiPortHole has to create a new Port in the bottom alias MPH.
	virtual PortHole& newConnection() = 0;

        // Return a PortHole.  For single portholes, it just returns
	// a reference to itself.  For multiporthole, it creates a new one.
        virtual PortHole& newPort() = 0;

	// Get the bottommost/innermost port of an alias chain.
	// In non-error cases, this will be a "real" port not an alias port.
	// For historical reasons it's called "realPort" not "getBottomAlias".
	GenericPort& realPort();

	// Same for use in const expressions
	const GenericPort& realPort() const;

	// Get the topmost/outermost port of an alias chain.
	// If an unaliased porthole is given, the same port is returned.
	GenericPort& getTopAlias();

	// Same for use in const expressions
	const GenericPort& getTopAlias() const;

	// Obtain immediate neighbors in alias chain, if any.
	// alias() is the next inner port, aliasFrom() the next outer.
	inline GenericPort* alias() const { return aliasedTo;}
	inline GenericPort* aliasFrom() const { return aliasedFrom;}

	// Delink me from any alias chain I may be in
	void clearAliases();

protected:
	// Make this port an alias for the specified port.
	// This is protected so that it cannot be used willy-nilly.
	// The public versions are exported by PortHole and MultiPortHole,
	// and are typed to enforce the constraint that a PortHole can
	// only alias to a PortHole and a MultiPortHole to a MultiPortHole.
	void setAlias (GenericPort& gp);

	// Relink any alias chain pointing to me to point to the given port
	// (adding it to the top of any extant chain for the port).
	// As above, type-safe versions are exported by PortHole/MultiPortHole.
	void moveMyAliasesTo (GenericPort& gp);

private:
	// Declared datatype of this porthole (may be ANYTYPE).
	// This is not necessarily what the connection will resolve to!
	DataType myType;

	// PortHole this is aliased to, if any (next inner alias-chain member)
	GenericPort* aliasedTo;

	// PortHole aliased to me, if any (next outer alias-chain member)
	GenericPort* aliasedFrom;

	// My type matches type of this port.
	// If not NULL, typePort pointers form a circular loop.
	GenericPort* typePortPtr;

	// My attributes
	bitWord attributeBits;
};

// predicate for a hidden porthole (or multiporthole)
inline int hidden(const GenericPort& p) { return p.attributes() & PB_HIDDEN; }


        //////////////////////////////////////////
        // class PortList
        //////////////////////////////////////////

// Used to store a list of PortHoles in a MultiPortHole
class PortList : private NamedObjList
{
	friend class PortListIter;
	friend class CPortListIter;
public:
        // Add PortHole to list
        inline void put(PortHole& p);

	// Find a port with the given name and return pointer
	inline PortHole* portWithName(const char* name);
	inline const PortHole* portWithName(const char* name) const;

	// Remove a port
	inline int remove(PortHole* p);

	NamedObjList::size;
	NamedObjList::initElements;
	NamedObjList::deleteAll;
        NamedObjList::initialize;
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
	MultiPortHole();

	void initialize();

	// make a bus connection with another multiporthole
	// 3/2/94 changed to support initDelayValues
	void busConnect (MultiPortHole&, int width, int delay = 0,
			 const char* initDelayValues = 0);

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
        inline int numberPorts() const {return ports.size();}

	// Return a new port for connections
	/* virtual */ PortHole& newConnection();

        // Add a new physical port to the MultiPortHole list
        /* virtual */ PortHole& newPort();

	// set alias for MultiPortHole
	inline void setAlias (MultiPortHole& blockPort) {
		GenericPort::setAlias (blockPort);
	}

	// move alias for MultiPortHole
	inline void moveMyAliasesTo (MultiPortHole& blockPort) {
		GenericPort::moveMyAliasesTo (blockPort);
	}

        // Print a description of the MultiPortHole
	/* virtual */ StringList print (int verbose = 0) const;

	// destructor
	~MultiPortHole();

protected:                           
        // List of ports allocated
        PortList ports;

	// Method for generating names for contained PortHoles
	const char* newName();

	// add a newly created port to the multiporthole
	virtual PortHole& installPort(PortHole& p);

	// set myMultiPortHole to this
	void letMeKnownToChild(PortHole& p);

	// delete all contained portholes
	void delPorts();
};

        //////////////////////////////////////////
        // class PortHole
        //////////////////////////////////////////

// Contains all the facilities of all PortHoles; base class
//  for all PortHoles
class PortHole : public GenericPort
{
	friend class Geodesic;	// allow Geodesic to access myPlasma
	friend class EventHorizon;	// access myBuffer
	friend class ToEventHorizon;	// access getParticle()
	friend class FromEventHorizon;	// access putParticle()
	friend class ClusterPort;

	// the following function may set indices
	friend int setPortIndices(Galaxy&);

	// the following function may set myMultiPortHole
	friend void MultiPortHole::letMeKnownToChild(PortHole&);

public:

	// Constructor: we come into the world disconnected and nameless,
	// without a parent.
	PortHole ();
	
	// Destructor: we disconnect ourselves and remove ourselves
	// from the parent Block
	~PortHole ();

        // Every PortHole must be initialized with the setPort function
        // Arguments are the name and type (see type.h for supported types).
        PortHole& setPort(const char* portName,
                          Block* parent,                // parent block pointer
                          DataType type = FLOAT,	// defaults to FLOAT
			  int n = 1);			// number xferred

	// Initialize when starting a simulation
	virtual void initialize();

	// Similar to initialize() but still keep its infrastructure.
	virtual void resetBufferValues();

	// Remove a connection, and optionally attempt to delete
	// the geodesic.  Arg is set to zero only when called from
	// the geodesic's destructor.  We do not delete "persistent"
	// geodesics.
	virtual void disconnect(int delGeo = 1);

	// Find the porthole that a connection to me should really connect to.
	/* virtual */ PortHole& newConnection();

        // Return a PortHole.
	// For simple PortHoles, just return a reference to self.
        /* virtual */ PortHole& newPort() { return *this; }

	// Return the porthole we are connected to (see below).
	inline PortHole* far() const { return farSidePort;}

        // Print a description of the PortHole
	/* virtual */ StringList print (int verbose = 0) const;

	// set alias for PortHole
	inline void setAlias (PortHole& blockPort) {
		GenericPort::setAlias (blockPort);
	}

	// move alias for PortHole
	inline void moveMyAliasesTo (PortHole& blockPort) {
		GenericPort::moveMyAliasesTo (blockPort);
	}

	// class identification
	int isA(const char*) const;

	// am I at wormhole boundary?  Disconnected ports return true,
	// since we implement clusters that way (disconnected ports
	// represent the cluster boundary)

	inline int atBoundary() const {
		return (farSidePort ? (isItInput() == farSidePort->isItInput()) : TRUE);
	}

	// Return me as an eventHorizon
	virtual EventHorizon* asEH();

	// Return me as a ClusterPort
	virtual ClusterPort* asClusterPort();

	// Can be used for things like inputing and output
	//  Particles.  These are currently do-nothing functions
	virtual void receiveData(); 
	virtual void sendData();

	// Operator to return Particles previously input or output
	// Argument is the delay in the past
        virtual Particle& operator % (int); // changed to virtual for MDSDF

        // return a StringList describing the current data in the port.
        virtual StringList getDataAsStrings ();

	// return the "resolved type", the type that the pair of connected
	// portholes have agreed to use.  This will never be ANYTYPE.
	// The result will be 0 (a null pointer) if type resolution has
	// not yet been performed.
	inline DataType resolvedType () const { return myResolvedType; }

	// return the "preferred type" of the porthole (never ANYTYPE).
	// The result will be 0 (a null pointer) if type resolution has
	// not yet been performed.
	inline DataType preferredType () const { return myPreferredType; }

	// function to resolve types during initialization.
	// Returns the resolved type of this porthole, or 0 on failure.
	// Probably this ought to be protected?
	virtual DataType setResolvedType();

	// return the number of tokens transferred per execution
	inline int numXfer() const { return numberTokens;}

	// return the number of particles on my Geodesic
	int numTokens() const;

	// return the number of initial delays on my Geodesic
	int numInitDelays() const;

        // return a string representing the initial delays on my Geodesic
        // 3/2/94 added
        const char* initDelayValues() const;

	// return pointer to my Geodesic
	inline Geodesic* geo() { return myGeodesic;}

	// index value
	inline int index() const { return indexValue;}

	// return the MultiPortHole that spawned this PortHole, or NULL
	// if there is no such MultiPortHole.

	inline MultiPortHole* getMyMultiPortHole() const { return myMultiPortHole; }

	// adjust the delay on the connection, with initial values
	virtual void setDelay (int numberDelays, 
                               const char* initDelayValues = 0);

	// Allocate a return a Geodesic compatible with this
	// type of PortHole
	virtual Geodesic* allocateGeodesic();

	// enable locking on access to the Geodesic and Plasma.  This is
	// appropriate for connections that cross thread boundaries.
	// assumption: initialize() has been called.

	void enableLocking(const PtGate& master);
	// the converse
	void disableLocking();

	int isLockEnabled() const;

protected:
	// Maintain pointer to Geodesic connected to this PortHole
	Geodesic* myGeodesic;

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

	// Number of Particles stored in the buffer each
	// time the Geodesic is accessed -- normally this is
	// one except for dataflow-type stars, where it is the number of
	// Particles consumed or generated
	int numberTokens;

	// get Particle from the Geodesic
	void getParticle();

	// put Particle to the Geodesic
	void putParticle();

	// clear Particle
	void clearParticle();

	// allocate Plasma (default method uses global Plasma)
	virtual int allocatePlasma();

	// alternate function allocates a local Plasma (for use in
	// derived classes)
	int allocateLocalPlasma();

	// delete Plasma if local; detach other end of connection
	// from Plasma as well.
	void deletePlasma();

	// Allocate new buffer
	void allocateBuffer();

	// function to determine preferred types during initialization.
	// Returns the preferred type of this porthole, or 0 on failure.
	// Protected, not private, so that subclasses that override
	// setResolvedType() can call it.
	DataType setPreferredType();

private:
	// Actual type of particles flowing through my geodesic.
	// 0 if not determined yet.
	DataType myResolvedType;

	// "Preferred" type of porthole --- intermediate step in
	// type resolution process.  See setResolvedType and friends.
	DataType myPreferredType;

	// index value, for making scheduler tables
	int indexValue;

	// Pointer to the MultiPortHole that spawned this PortHole,
	// if there is one.  Otherwise, NULL
	MultiPortHole* myMultiPortHole;

	// Two recursive phases of preferred-type assignment
	DataType assignPass1();
	DataType assignPass2();
	// Find the "true" far end, ignoring any intervening event horizons.
	PortHole* findFarEnd() const;
};

// PortList methods.  They are here because they cannot appear until
// class PortHole is declared.

inline void PortList::put(PortHole& p) {NamedObjList::put(p);}

inline 	const PortHole* PortList::portWithName(const char* name) const {
	return (const PortHole*)objWithName(name);
}

inline 	PortHole* PortList::portWithName(const char* name) {
	return (PortHole*)objWithName(name);
}

inline int PortList::remove(PortHole* p) { return NamedObjList::remove(p);}

// The following generic type is good enough to use in galaxies.
// It always has an alias.
// The "isIt" functions work by asking the alias.

        //////////////////////////////////////////
        // class GalPort
        //////////////////////////////////////////

class GalPort : public PortHole {
public:
	GalPort(PortHole& a);
	GalPort() {}
	~GalPort();
	int isItInput() const;
	int isItOutput() const;
};

        //////////////////////////////////////////
        // class PortListIter
        //////////////////////////////////////////

// An iterator for PortLists, non-const version
class PortListIter : public NamedObjListIter {
public:
	PortListIter(PortList& plist) : NamedObjListIter (plist) {}
	inline PortHole* next() { return (PortHole*)ListIter::next();}
	inline PortHole* operator++(POSTFIX_OP) { return (PortHole*)ListIter::next();}
	NamedObjListIter::reset;
	NamedObjListIter::remove;
};


        //////////////////////////////////////////
        // class GalMultiPort
        //////////////////////////////////////////

class GalMultiPort : public MultiPortHole {
public:
	// a GalMultiPort always has an alias
	GalMultiPort(MultiPortHole& a);

	// If you want to defer creating the alias to an alias() call
	GalMultiPort() {}

	~GalMultiPort();

	// queries pass through to the inside
	int isItInput() const;
	int isItOutput() const;

	// when making a new port, create it both locally and in the
	// next lower alias, and connect the two together.
	/* virtual */ PortHole& newPort();
};


        //////////////////////////////////////////
        // class MPHList
        //////////////////////////////////////////

// Used to store a list of MultiPortHoles
class MPHList : private NamedObjList
{
	friend class MPHListIter;
	friend class CMPHListIter;
public:
        // Add MultiPortHole to list
        inline void put(MultiPortHole& p) {NamedObjList::put(p);}
	// Find a multiport with the given name and return pointer
	inline MultiPortHole* multiPortWithName(const char* name) {
		return (MultiPortHole*)objWithName(name);
	}
	inline const MultiPortHole* multiPortWithName(const char* name) const {
		return (const MultiPortHole*)objWithName(name);
	}

	NamedObjList::size;
	NamedObjList::initElements;
	NamedObjList::deleteAll;
};

        //////////////////////////////////////////
        // class MPHIter
        //////////////////////////////////////////

// Iterator for MultiPortHoles
class MPHIter : public PortListIter {
public:
	MPHIter(MultiPortHole& mph) : PortListIter (mph.ports) {}
};

        //////////////////////////////////////////
        // class MPHListIter
        //////////////////////////////////////////

// An iterator for MPHLists, non-const version
class MPHListIter : private NamedObjListIter {
public:
	MPHListIter(MPHList& plist) : NamedObjListIter (plist) {}
	inline MultiPortHole* next() { return (MultiPortHole*)NamedObjListIter::next();}
	inline MultiPortHole* operator++(POSTFIX_OP) { return next();}
	NamedObjListIter::reset;
};

#endif
