#include "Connect.h"
#include "CircularBuffer.h"
#include "Block.h"
#include "StringList.h"
#include "Output.h"
#include "Particle.h"
#include "Geodesic.h"
#include "Plasma.h"
#include "miscFuncs.h"
 
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 1/17/90
 Revisions:

Code for functions declared in Connect.h

**************************************************************************/

// Methods for CircularBuffer


const char* CircularBuffer :: errMsgString = "";

CircularBuffer :: CircularBuffer(int i)
{
        dimen = i;
        current = 0;
        buffer = new Particle*[dimen];
	for(int j = dimen-1; j>=0; j--)
		buffer[j] = NULL;
}

CircularBuffer :: ~CircularBuffer()
{
// If there are any particles, return them to correct Plasma
	for (int i = size(); i>0; i--) {
		Particle* q = *next();
		if (q) q->die();
	}
        delete buffer;
}

// Find the position in the buffer i in the past relative to current
// This method does not change current
// Returns NULL on access error
Particle** CircularBuffer :: previous(int i) const
{
        if (i > dimen-1)  {
		errMsgString = "Access with too large a delay";
		return 0;
	}
	if (i < 0) {
		errMsgString = "Access with negative delay";
		return 0;
	}
	if (i > current)
		// Keep (current-i) from being negative
		i -= dimen;

        return buffer + ((current - i) % dimen);
}

// Small virtual methods
int GenericPort :: isItInput () { return FALSE;}
int GenericPort :: isItOutput () { return FALSE;}
int GenericPort :: isItMulti () { return FALSE;}

PortHole& GenericPort :: newConnection () {
	// my apologies for this horrible cast
	return *(PortHole *)&GenericPort::realPort();
}

// inheritTypeFrom maintains a circular list of typePortPtr pointers.
// only this function and the destructor alters typePortPtr.
void GenericPort :: inheritTypeFrom(GenericPort& p) {
	typePortPtr = &p;
// case 1: no pre-existing circle.  Make one and return.
	if (!p.typePortPtr) {
		p.typePortPtr = this;
		return;
	}
// case 2: a pre-existing circle.  Search for the link back to p
// and make it point to me instead.
	GenericPort* q = p.typePortPtr;
	while (q->typePortPtr != &p)
		q = q->typePortPtr;
	q->typePortPtr = this;
	return;
}

// destructor remove myself from the circle but preserve a smaller circle
GenericPort :: ~GenericPort () {
	if (!typePortPtr) return;
	GenericPort* q = typePortPtr;
	while (q->typePortPtr != this) q = q->typePortPtr;
	q->typePortPtr = typePortPtr;
}

// The connect method
// This method is virtual and may be overridden

void GenericPort :: connect(GenericPort& destination,int numberDelays)
{
	// Resolve any aliases and MultiPortHole stuff:
	// newConnection is a virtual function that does the right
	// thing for all types of PortHoles.
	PortHole& realSource = newConnection();
	PortHole& realDest = destination.newConnection();

	Geodesic* geo = realSource.allocateGeodesic();
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

	// Set the number of delays
	geo->numInitialParticles = numberDelays;

	return;
}

// This is not a GenericPort method because the concept of disconnecting
// a multiporthole is ambiguous.  Since fancier geodesics work differently,
// this is virtual -- redefined for some domains.
void PortHole :: disconnect() {
	if (!farSidePort) return;
	// free up geodesic
	farSidePort->myGeodesic = 0;
	delete myGeodesic;
	myGeodesic = 0;
	farSidePort = 0;
	return;
}

// Porthole destructor.
PortHole :: ~PortHole() {
	disconnect();
	delete myBuffer;
}

// small virtual functions for PortHole, InPortHole, OutPortHole

int InPortHole :: isItInput() { return TRUE;}
int OutPortHole :: isItOutput() { return TRUE;}
void PortHole :: beforeGo () { return;}
void PortHole :: afterGo () { return;}

// return number of tokens waiting on Geodesic
int PortHole :: numTokens() const { return myGeodesic->size();}

void PortHole :: allocateBuffer()
{
	// If there is a buffer, release its memory
	delete myBuffer;

	// Allocate new buffer, and fill it with initialized Particles
	myBuffer = new CircularBuffer(bufferSize);
        for(int i = myBuffer->size(); i>0; i--) {
                Particle** p = myBuffer->next();
                *p = myPlasma->get();
	}
}

PortHole& PortHole :: setPort(const char* s,
                              Block* parent,
                              dataType t = FLOAT) {
	GenericPort::setPort (s, parent, t);
	numberTokens = 1;
	bufferSize = numberTokens;
	if (t != ANYTYPE)
		myPlasma = Plasma::getPlasma(t);
        return *this;
}

// Print a Generic Port
StringList
GenericPort :: printVerbose () {
        StringList out;
 
        if(isItInput())
           out = "      Input ";
        else if(isItOutput())
           out = "      Output ";
        
        out += "PortHole: ";
        out += readFullName();
        out += "\n";
        
        if(alias != NULL) {
           GenericPort& eventualAlias = realPort();
           out += "       Aliased to: ";
           out += eventualAlias.readFullName();
           out += "\n";
	}
	return out;
}

StringList
PortHole :: printVerbose () {
	StringList out;
	out = GenericPort::printVerbose();
	if (alias == NULL) {
           if (far() != NULL) {
              out += "    Connected to port: ";
              out += far()->readFullName();
              out += "\n";
              }
           else
              out += "    Not connected.\n";
        }
        return out;
}

int MultiPortHole :: isItMulti() { return TRUE;}

void MultiPortHole :: initialize() { reset();}

// this is not really a do-nothing: it destroys its member object "ports".
MultiPortHole :: ~MultiPortHole() {}

StringList
MultiPortHole :: printVerbose () {
	StringList out;
	out = "Multi ";
	out += GenericPort::printVerbose();
	out += "This MultiPortHole contains ";
	out += numberPorts();
	out += " PortHoles.\n";
// We don't add stuff on the PortList, since that will be printed
// out by the enclosing Block::printPorts.
	return out;
}

// define a marker value to prevent infinite recursion
Plasma* const Mark = (Plasma*)1;

// The setPlasma function's job is to propagate types all around the
// structure.  It is, unfortunately, necessarily complex.  It supports
// several functions:
// 1) The allowance and correct resolution of ANYTYPE (for Fork, Printer,
//    type stars)
// 2) Allowing ports with different types to be connected together; the
//    input porthole determines what Plasma to use
// 3) The use of typePort() to require that several PortHoles have the
//    same type
// 4) Handle wormhole boundary conditions, where far() doesn't quite
//    mean the same thing.
//
// In some cases the type is really undefined.  Consider this universe
// (using interpreter syntax)
//
// (star f Fork) (star p Printer)
// (connect f output f input 1)
// (connect f output p input)
//
// There are no types anywhere in the system.

Plasma*
PortHole :: setPlasma (Plasma* useType) {
// return immediately if useType matches myPlasma and non-null.
	if (far() == NULL) return myPlasma;
// check for infinite recursion
	
	if (myPlasma == Mark) return 0;

// I am allowed to change my type only if I am an output porthole.
// This happens if, say, an output of type FLOAT feeds an input of
// type Complex.
	if (useType) {
		if (useType == myPlasma) return useType;
		if (!myPlasma || isItOutput()) {
			myPlasma = useType;
			// following recursion ends when we get to ourself
			if (typePort()) typePort()->setPlasma(useType);
		}
		else {
			Error::abortRun(*this, ": unresolvable type conflict");
			return myPlasma;
		}
	}
// If it is an input PortHole
	else if (isItInput()) {
// If my type isn't known try to set it.
		if (!myPlasma) {
			if (typePort()) {
				myPlasma = Mark;
				myPlasma = typePort()->setPlasma();
			}
			// If still not set, try the connected output
			if (!myPlasma) {
				myPlasma = Mark;
				myPlasma = far()->setPlasma();
			}
		}
	} 
// If it is an output PortHole.
	else {	
		// first, far() has known type: use it
		if (far()->myPlasma && far()->myPlasma != Mark) {
			myPlasma = far()->myPlasma;
		}
		// or, far() has typePort: set far() and use it
		else if (far()->typePort()) {
			Plasma* save = myPlasma;
			myPlasma = Mark;
			myPlasma = far()->setPlasma();
			if (!myPlasma) myPlasma = save;
			if (myPlasma) far()->setPlasma(myPlasma);
		}
		// still not set: look for typePort
		if (myPlasma == 0 && typePort()) {
			myPlasma = Mark;
			myPlasma = typePort()->setPlasma();
		}
	}
	if (myPlasma) {
		if (typePort()) typePort()->setPlasma(myPlasma);
	}
	return myPlasma;
}

// Function to get plasma type for a MultiPortHole.
Plasma*
MultiPortHole :: setPlasma (Plasma* useType) {
	return typePort() ? typePort()->setPlasma(useType) : 0;
}

void PortHole :: initialize()
{
	// set plasma if not set
	if (!setPlasma ()) {
		Error::abortRun (*this, ": can't determine dataType");
		return;
	}

	// allocate buffer if not allocated or wrong size
	if (myBuffer == NULL || bufferSize != myBuffer->size())
		allocateBuffer ();

	// initialize buffer
	for(int i = myBuffer->size(); i>0; i--) {
		Particle** p = myBuffer->next();
		// Initialize particles on the buffer, so when we
		// restart they do not contain old data
		(*p)->initialize();
	}
	// If this is an output PortHole or EventHorizon, initialize myGeodesic
	if( (isItOutput() || isItInput() == far()->isItInput()) && myGeodesic) 
		myGeodesic->initialize();
}

void PortHole :: setMaxDelay(int delay)
{
	// This method can be used to change the maximum delay
	// or to set it based on a parameter

	bufferSize = delay + numberTokens;
}

Particle& PortHole ::  operator % (int delay)
{
	Particle** p = myBuffer->previous(delay);
	if(p == NULL || *p == NULL) {
		Error::abortRun(*this,CircularBuffer::errMsg());
		// kludge -- gotta get a particle somehow so we don't
		// dump core.
		return *myPlasma->get();
	}
        return **p;
}

MultiPortHole& MultiPortHole :: setPort(const char* s,
                              Block* parent,
                              dataType t = FLOAT) {
	GenericPort::setPort (s, parent, t);
        return *this;
}

// Expand aliases.  This isn't inlinable by G++ since it's recursive
GenericPort& GenericPort :: realPort() {
	if (alias == NULL) return *this;
	return alias->realPort();
}

// Method to generate names for new portholes.  Names are of the
// form "name#num", where name is the MultiPortHole name and num
// is a sequence number.  The number equals the current number of
// contained ports.
char*
MultiPortHole :: newName () {
	char buf[512];
	sprintf (buf, "%s#%d", readName(), ports.size());
// save the string on the heap.  Disadvantage: multiple copies of "input#1"
	return savestring (buf);
}

// Add a new PortHole to the MultiPortHole.  install is provided so
// we can do multiports for specific domains.
PortHole& MultiPortHole :: installPort(PortHole& p) {
	ports.put(p);
	parent()->addPort(p.setPort(newName(), parent(), type));
	p.inheritTypeFrom(*this);
	return p;
}

PortHole& MultiPortHole :: newPort() {
	return installPort(*new PortHole);
}

// Return a PortHole for a new connection.  We return the first available
// unconnected PortHole.  If there are none, we make a new one.

PortHole& MultiPortHole :: newConnection() {
	// resolve aliases
	if (alias) return realPort().newConnection();

	// find an unconnected porthole
	ports.reset();
	for (int i = ports.size(); i > 0; i--) {
		PortHole& p = ports++;
		if (p.far() == NULL) return p;
	}

	// no disconnected ports, make a new one.
	return newPort();
}

Particle* Geodesic::get()
{
	if (sz > 0) {
		sz--;
		return ParticleStack::get();
	}
	else return 0;
}

void Geodesic :: initialize()
{
	// Remove any Particles residing on the Geodesic,
	// and put them in Plasma
	freeup();

	// Initialize the buffer to the number of Particles
	// specified in the connection; note that these Particles
	// are initialized by Plasma
	for(int i=numInitialParticles; i>0; i--) {
		Particle* p = originatingPort->myPlasma->get();
		putTail(p);
	}
	sz = numInitialParticles;
	// TO DO: Allow Particles in the Geodesic to be
	// initialized to specific values
}


Geodesic* PortHole :: allocateGeodesic () {
	return new Geodesic;
}

/*
 * additional methods for PortHoles
 */


/* Particle movement methods */


void PortHole :: getParticle()
{
// It is assumed this routine is called before a Star executes...
// It gets numberTokens Particles from the Geodesic and stores them
// in the buffer, then setting current time to the last Particle input

	for(int i = numberTokens; i>0; i--)
	{
		// Move the current time pointer in the buffer
		Particle** p = myBuffer->next();

                // Put current Particle back into Plasma  to be
		// recycled back to some OutSDFPort
		myPlasma->put(*p);
 
		// Get another Particle from the Geodesic
        	*p = myGeodesic->get();
		if (*p == NULL) {
			Error::abortRun(*this,"Attempt to read from empty geodesic");
			return;
		}
	}
}

void PortHole :: clearParticle()
{
	Particle** p;

// It is assumed this routine is called before a Star executes....
// It moves current by numberTokens in the buffer, initializing
//  each Particle; after the Star executes these Particles will
//  contain data destined for another Star, and will be launched
//  into the Geodesic

// After this routine executes, the buffer current will point to
// the last Particle, so the operator% references Particles relative
// to this time

	for(int i = numberTokens; i>0; i--) {
	
		// Get and initialize next Particle
        	p = myBuffer->next();
		(*p)->initialize();
	}
}

void PortHole :: putParticle()
{
	Particle** p;

// This method is called after go(); the buffer now contains numberTokens
// Particles that are to be send to the output Geodesic
// We send copies to prevent the Star from modifying Particles in the
// Geodesic or InPortHole on the other side

	// Back up in the buffer by numberTokens
	for(int i = numberTokens; i>0; i--)
        	p = myBuffer->last();

	// Now move forward numberTokens times, sending copy of
	// Particle to Geodesic
	for(i = numberTokens; i>0; i--) {
		p = myBuffer->next();
		
		// Get Particle from Plasma
		Particle* pp = myPlasma->get();

		// Copy from the buffer to this Particle
		*pp = **p;

		// Launch this Particle into the Geodesic
		myGeodesic->put(pp);
		}

	// Note that the Particle is left in the buffer to be
	// accessed as a past Particle
}
