#include "Connect.h"
#include "Block.h"
#include "StringList.h"
#include "Output.h"
 
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 1/17/89
 Revisions:

Code for functions declared in Connection.h

**************************************************************************/

extern Error errorHandler;
extern PlasmaList plasmaList;

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
        current = ++current % dimen;
        return buffer+current;
}

Pointer* CircularBuffer :: last()
{
        current = --current % dimen;
        return buffer+current;
}

// Find the position in the buffer i in the past relative to current
// This method does not change current
Pointer* CircularBuffer :: previous(int i)
{
        if(i > dimen-1)
		errorHandler.error(
                "CircularBuffer: Access with too large a delay\n"
		);
	else if(i < 0)
		 errorHandler.error(
                "CircularBuffer: Access with negative delay\n"
                );
	else if(i > current)
		// Keep (current-i) from being negative
		i -= dimen;

        return buffer + ((current - i) % dimen);
}

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
        myGeodesic = NULL;
	myBuffer = NULL;
	bufferSize = numberTokens;
	numberTokens = 1;
        farSidePort = NULL;
	if (t != ANYTYPE)
		myPlasma = plasmaList.getPlasma(t);
        return *this;
}

PortHole& SDFPortHole :: setPort (
			     const char* s,
                             Block* parent,
                             dataType t = FLOAT,
                             unsigned numTokens = 1,
			     unsigned delay = 0)
{
	// Initialize PortHole
        PortHole::setPort(s,parent,t);

	// Based on the delay, we allocate a Buffer assuming no
	// past Particles are allocated
        numberTokens = numTokens;
	// The number of Particles the buffer has to hold is:
	//		numberTokens current and future Particles
	//		delay past Particles
	bufferSize = numberTokens + delay;

        return *this;
}

// Print a Generic Port
GenericPort :: operator StringList () {
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

PortHole :: operator StringList () {
	StringList out;
	out = GenericPort:: operator StringList ();
	if (alias == NULL) {
           if (farSidePort != NULL) {
              out += "    Connected to port: ";
              out += farSidePort->readFullName();
              out += "\n";
              }
           else
              out += "    Not connected.\n";
        }
        return out;
}

MultiPortHole :: operator StringList () {
	StringList out;
	out = "Multi ";
	out += GenericPort:: operator StringList ();
	out += "This MultiPortHole contains ";
	out += numberPorts();
	out += " PortHoles.\n";
// Should add stuff on the PortList
	return out;
}

Plasma*
PortHole :: setPlasma () {
	// first case: my type is known
	if (myPlasma) return myPlasma;
	// second case: I've been told where to look for the type
	if (typePort)
		myPlasma = typePort->setPlasma();
	// third case: I'm an input porthole, use type of connected
	// output porthole.
	// fourth case: I'm an output, and the connected input
	// specifies a type
	if (farSidePort && (isItInput() || farSidePort->myPlasma))
		myPlasma = farSidePort->setPlasma();
	if (myPlasma) return myPlasma;
	errorHandler.error ("Can't determine type of ",readFullName());
	return 0;
}

// Function to get plasma type for a MultiPortHole.
// The algorithm is to use the type of the first contained PortHole
// if typePort is null.  This means that, for an adder star, say,
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
	// If this is an output PortHole, initialize myGeodesic
	if( isItOutput() ) myGeodesic->initialize();
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
        if(*p == NULL)
		 errorHandler.error(
                "Attempt to access nonexistent input Particle"
			);
        return *(Particle*)*p;
}

void InSDFPort :: beforeGo()
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

void OutSDFPort :: beforeGo()
{
	Pointer* p;

// It is assumed this routine is called before a Star executes....
// It moves current by numberTokens in the buffer, initializing
//  each Particle; after the Star executes these Particles will
//  contain data destined for another Star, and will be launched
//  into the Geodesic

// For this case, the buffer always stores the same set of
// Particles -- copies of these Particles are launched into the
// Geodesic so that there is no possibility of modifying a Particle
// that resides in the Geodesic or InSDFPort on the other side

// After this routine executes, the buffer current will point to
// the last Particle, so the operator% references Particles relative
// to this time

	for(int i = numberTokens; i>0; i--) {
	
		// Get and initialize next Particle
        	p = myBuffer->next();
		((Particle*)*p)->initialize();
		}
}

void OutSDFPort :: afterGo()
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

Particle& InOtherPort :: operator ++ ()
{
        Pointer* p = myBuffer->next();

	if(*p != NULL)
		// Put current Particle back into Plasma
		myPlasma->put((Particle*)*p);

	// Get a new input from the Geodesic
	*p = myGeodesic->get();
	return *(Particle*)*p;
}
 
Particle& InOtherPort ::  operator -- ()
{
	Pointer* p = myBuffer->here();

	if(*p == NULL) errorHandler.error(
		"Attempt to push back nonexistent Particle"
		);
	else 	{
		myGeodesic->pushBack((Particle*)*p);
		myBuffer->last();
		}
}


Particle& OutOtherPort :: operator ++ ()
{
	Pointer* p = myBuffer->next();

	*p = myPlasma->get();
	myGeodesic->put((Particle*)*p);
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


MultiPortHole& MultiSDFPort :: setPort (const char* s,
                             Block* parent,
                             dataType t = FLOAT,
                             unsigned numTokens = 1) {
        MultiPortHole::setPort(s,parent,t);
        numberTokens = numTokens;
        return *this;
}

PortHole& MultiInSDFPort :: newPort () {
        InSDFPort* newport = new InSDFPort;
        ports.put(*newport);
        parent()->
            addPort(newport->
                        setPort(newName(), parent(), type, numberTokens));
	newport->typePort = typePort;
        return *newport;
}
 
 
PortHole& MultiOutSDFPort :: newPort () {
        OutSDFPort* newport = new OutSDFPort;
        ports.put(*newport);
        parent()->
            addPort(newport->
                        setPort(newName(), parent(), type, numberTokens));
	newport->typePort = typePort;
        return *newport;
}

Particle* Geodesic::get()
{
	if(size() == 0) errorHandler.error(
		"Geodesic: Attempt to access empty Geodesic"
		);
	return (Particle*)popTop();
}

void Geodesic :: initialize()
{
	// Remove any Particles residing on the Geodesic,
	// and put them in Plasma
	for(int i=size(); i>0; i--) {
		Particle* p = get();
		destinationPort->myPlasma->put(p);
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
