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
 Date of creation: 1/17/90
 Revisions:

Code for functions declared in Connect.h

**************************************************************************/

extern Error errorHandler;
extern PlasmaList plasmaList;

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
		myPlasma = plasmaList.getPlasma(t);
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
	// first case: my type is known, and I'm an input.  Leave alone.
	if (myPlasma && isItInput()) return myPlasma;
	// second case: I've been told where to look for the type
	if (typePort)
		myPlasma = typePort->setPlasma();
	// third case: I'm an input porthole, use type of connected
	// output porthole.
	// fourth case: I'm an output, and the connected input
	// specifies a type
	if (far() && (isItInput() || far()->myPlasma))
		myPlasma = far()->setPlasma();
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
	if( isItOutput() && myGeodesic) myGeodesic->initialize();
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
