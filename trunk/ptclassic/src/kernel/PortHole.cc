#include "Connect.h"
#include "Block.h"
#include "StringList.h"
#include "Output.h"
#include "Particle.h"
 
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

CircularBuffer :: CircularBuffer(int i)
{
        dimen = i;
        current = 0;
        buffer = new Pointer[dimen];
	for(int j = dimen-1; j>=0; j--)
		buffer[j] = NULL;
}

CircularBuffer :: ~CircularBuffer()
{
        delete buffer;
}

Pointer* CircularBuffer :: here()
{
        return buffer+current;
}

Pointer* CircularBuffer :: next()
{
	++current;
	if (current >= dimen) current = 0;
        return buffer+current;
}

Pointer* CircularBuffer :: last()
{
	--current;
	if (current < 0) current = dimen - 1;
        return buffer+current;
}

// Find the position in the buffer i in the past relative to current
// This method does not change current
Pointer* CircularBuffer :: previous(int i)
{
        if(i > dimen-1)
		Error::abortRun(
                "CircularBuffer: Access with too large a delay\n"
		);
	else if(i < 0)
		Error::abortRun(
                "CircularBuffer: Access with negative delay\n"
                );
	else if(i > current)
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

Plasma* GenericPort :: setPlasma() { return 0;}

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
	
void PortHole :: allocateBuffer()
{
	// If there is a buffer, release its memory
	if(myBuffer != NULL) {
		// Release any Particles on the buffer to Plasma
        	// before deleting it -- avoids dangling Particles!
        	for(int i = myBuffer->size(); i>0; i--) {
			Pointer* p = myBuffer->next();
			myPlasma->put((Particle*)*p);
			}

		// Now release the memory and re-allocate myBuffer
		delete myBuffer;
		}

	// Allocate new buffer, and fill it with initialized Particles
	myBuffer = new CircularBuffer(bufferSize);
        for(int i = myBuffer->size(); i>0; i--) {
                Pointer* p = myBuffer->next();
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

void MultiPortHole :: initialize() { ports.initialize();}

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

Plasma*
PortHole :: setPlasma () {
	// zeroth case: disconnected porthole.  Return what I am.
	if (far() == NULL) return myPlasma;

	// If it is an input PortHole
	if (isItInput()) {
		// first, my type is known.
		if (myPlasma) return myPlasma;
		// second, I am of ANYTYPE, look for typePort or far()
		if (typePort) myPlasma = typePort->setPlasma();
		else	      myPlasma = far()->setPlasma();
	} 
	// If it is an output PortHole.
	else {	
		// first, far() has known type and not on wormhole boundary
		if (far()->myPlasma && far()->isItInput()) {
			myPlasma = far()->myPlasma;
			return myPlasma;
		}
		// or, far() has typePort and not on wormhole boundary
		if (far()->typePort && far()->isItInput()) {
			myPlasma = far()->typePort->setPlasma();
		}
		// second, look for typePort (ANYTYPE or on wormhole boundary)
		if (typePort)
			myPlasma = typePort->setPlasma();
		// third, deal with rare case : all portholes around wormhole
		//	  boundary are of ANYTYPE.
		if (myType() == ANYTYPE && far()->isItOutput()) {
			PortHole* p = (PortHole*) far()->typePort;
			// if far()->typePort is defined and output, and
			// its far() has a known type.
			if (p && p->isItOutput() && p->far()->myPlasma)
				myPlasma = p->far()->myPlasma;
		}
	}
	if (myPlasma) return myPlasma;
	StringList msg = "Can't determine dataType of ";
	msg += readFullName();
	Error::abortRun (msg);
	return 0;
}

// Function to get plasma type for a MultiPortHole.
// The algorithm is to use the type of the first contained PortHole
// if typePort is null.  This means that, for an ANYTYPE adder star, say,
// the type of the output will be the type of the first input, which
// may not be what is desired.
Plasma*
MultiPortHole :: setPlasma () {
	if (typePort) return typePort->setPlasma();
	// call setPlasma the first contained PortHole to get the value.
	reset();
	return (ports++).setPlasma();
}

void PortHole :: initialize()
{
	// set plasma if not set
	setPlasma ();

	// allocate buffer if not allocated or wrong size
	if (myBuffer == NULL || bufferSize != myBuffer->size())
		allocateBuffer ();

	// initialize buffer
	for(int i = myBuffer->size(); i>0; i--) {
		Pointer* p = myBuffer->next();
		// Initialize particles on the buffer, so when we
		// restart they do not contain old data
		((Particle*)*p)->initialize();
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
	Pointer* p = myBuffer->previous(delay);
        if(*p == NULL) {
		StringList msg = readFullName();
		msg += "Attempt to access nonexistent input Particle";
		Error::abortRun(msg);
// kludge -- gotta get a particle somehow
		return *myPlasma->get();
	}
        return *(Particle*)*p;
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
	char* newname = new char[strlen(buf)+1];
	return strcpy (newname, buf);
}

// Add a new PortHole to the MultiPortHole.
PortHole& MultiPortHole :: newPort() {
        PortHole* newport = new PortHole;
        ports.put(*newport);
        parent()->addPort(newport->setPort(newName(), parent(), type));
	newport->typePort = typePort;
        return *newport;
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
	if(size() == 0) {
		Error::abortRun("Geodesic: Attempt to access empty Geodesic");
		return 0;
	}
	return (Particle*)popTop();
}

void Geodesic :: initialize()
{
	// Remove any Particles residing on the Geodesic,
	// and put them in Plasma
	for(int i=size(); i>0; i--) {
		Particle* p = get();
		p->die();
	}

	// Initialize the buffer to the number of Particles
	// specified in the connection; note that these Particles
	// are initialized by Plasma
	for(i=numInitialParticles; i>0; i--) {
		Particle* p = originatingPort->myPlasma->get();
		put(p);

	// TO DO: Allow Particles in the Geodesic to be
	// initialized to specific values
		}
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
		Pointer* p = myBuffer->next();

                // Put current Particle back into Plasma  to be
		// recycled back to some OutSDFPort
		myPlasma->put((Particle*)*p);
 
		// Get another Particle from the Geodesic
        	*p = myGeodesic->get();
		}
}

void PortHole :: clearParticle()
{
	Pointer* p;

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
		((Particle*)*p)->initialize();
		}
}

void PortHole :: putParticle()
{
	Pointer* p;

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
		*pp = *(Particle*)*p;

		// Launch this Particle into the Geodesic
		myGeodesic->put(pp);
		}

	// Note that the Particle is left in the buffer to be
	// accessed as a past Particle
}
