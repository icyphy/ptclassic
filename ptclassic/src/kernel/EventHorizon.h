#ifndef _EventHorizon_h
#define _EventHorizon_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "NamedObj.h"
#include "DataStruct.h"
#include "PortHole.h"

enum inOutType {IN, in, OUT, out};
class Star;
class Wormhole;

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 5/31/90
 Date of revision: 9/16/92
 Revisions:
	EventHorizon is defined.
******************************************************************/

/*********************************************************************

 EventHorizon:

 For Wormholes, we need another class of PortHoles that will be multiply
 derived from a domain-specific Porthole class and common EventHorizon
 class.

 *********************************************************************/

        //////////////////////////////////////////
        // class EventHorizon
        //////////////////////////////////////////

//  This PortHole links two PortHoles inside and outside a Wormhole.
class EventHorizon 
{
friend class Wormhole;
friend class ToEventHorizon;

public:
	// constructor
	// We never use plain EventHorizons, we always have
	// SDFToUniversal: public ToUniversal, public InSDFPort
	// SDFfromUniversal: public FromUniversal, public OutSDFPort
	// SDFToUniversal::SDFToUniversal() : ToEventHorizon(this) {...}

	EventHorizon(PortHole* self): selfPort(self) {}

	// virtual destructor
	virtual ~EventHorizon();

	// return myself as a porthole
	PortHole* asPort() { return selfPort; }
	PortHole* ghostAsPort() { return ghostPort->asPort(); }

	// Connect two EventHorizons.
	virtual void ghostConnect(EventHorizon& to );

	// is it Input or Output? -- depends on the location.
	virtual int isItInput() const;
	virtual int isItOutput() const;

	// set ports
	virtual void setEventHorizon(inOutType inOut, const char* portName, 
		Wormhole* parentWormhole, Star* parentStar,
		DataType type = FLOAT, unsigned numTokens = 1 );

	// return the timeMark
	double getTimeMark() { return timeMark; }
	void setTimeMark(double d) { timeMark = d; }

	// virtual initialize
	virtual void initialize();

protected:
	// Access the myBuffer of the porthole
	CircularBuffer* buffer() { return asPort()->myBuffer; }

	// Transfer data from another EventHorizon, for use in
	// ToEventHorizon::transferData
	void moveFromGhost(EventHorizon& from, int numParticles);

	// We need another set of connection information
	// between boundary and inside of the wormhole.
	// ghostPort points to the other EventHorizon in the pair
	// that interfaces between two domains.

	EventHorizon* ghostPort;
	
	// Is data token new?
	int tokenNew;

	// Which wormhole it is in.
	Wormhole* wormhole;

	// TimeMark of the current data, which is necessary for interface
	// of two domains. 
	double timeMark;
private:
	// my direction
        int inOrOut;

	// myself as a Porthole
	PortHole* selfPort;

};

        //////////////////////////////////////////
        // class ToEventHorizon
        //////////////////////////////////////////

// Input EventHorizon
class ToEventHorizon : public EventHorizon
{
public:
	ToEventHorizon(PortHole* p) : EventHorizon(p) {}

	void initialize(); 
protected:

	// trasfering data from outside to Universal EventHorizon
	void getData() ;

	// transfer data from Universal EventHorizon to ghostPort.
	void transferData();
};

        //////////////////////////////////////////
        // class FromEventHorizon
        //////////////////////////////////////////

// Output EventHorizon
class FromEventHorizon : public EventHorizon
{
friend class Wormhole;
public:
	FromEventHorizon(PortHole* p) : EventHorizon(p) {}

	void initialize(); 
protected:

	//transfer data from Universal EventHorizon to outside
	void putData() { asPort()->putParticle();
			 tokenNew = FALSE; }

	// fire ghostPort :: grabData to get Data
	void transferData();

	// make sure this port is ready for inside Galaxy if it is an input.
	// if ready, set up the stopping condition for the inner-domain.
	virtual int ready();
			   
};

        //////////////////////////////////////////
        // class WormMultiPort
        //////////////////////////////////////////

// This class is used for Wormholes that have multiportholes in the
// inner galaxy.  The WormMultiPort acts as an alias on the outside,
// and it correctly creates pairs of EventHorizons to perform the
// interface.

class WormMultiPort : public MultiPortHole {
	Wormhole* worm;
public:
	WormMultiPort(Wormhole* w, GenericPort& a): worm(w) {
		GenericPort::setAlias(a);
	}
	int isItInput() const;
	int isItOutput() const;

	PortHole& newPort();
};

#endif
