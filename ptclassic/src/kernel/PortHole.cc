static const char file_id[] = "PortHole.cc";
#ifdef __GNUG__
#pragma implementation
#pragma implementation "CircularBuffer.h"
#endif

#include "PortHole.h"
#include "CircularBuffer.h"
#include "Block.h"
#include "Error.h"
#include "Particle.h"
#include "Geodesic.h"
#include "Plasma.h"
#include "miscFuncs.h"
#include "GalIter.h"
#include "EventHorizon.h"
 
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 1/17/90
 Revisions:

Code for functions declared in PortHole.h and CircularBuffer.h

**************************************************************************/

// Methods for CircularBuffer


const char* CircularBuffer :: errMsgString = "";

CircularBuffer :: CircularBuffer(int i)
{
        dimen = i;
        current = 0;
        LOG_NEW; buffer = new Particle*[dimen];
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
        LOG_DEL; delete buffer;
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

// Attributes for portholes
extern const Attribute P_HIDDEN = {PB_HIDDEN,0};
extern const Attribute P_VISIBLE = {0,PB_HIDDEN};

// constructor
GenericPort :: GenericPort () : myType(ANYTYPE),aliasedTo(0),typePortPtr(0),
	aliasedFrom(0), attributeBits(0) {}

// Small virtual methods
int GenericPort :: isItInput () const { return FALSE;}
int GenericPort :: isItOutput () const { return FALSE;}
int GenericPort :: isItMulti () const { return FALSE;}

GalPort :: ~GalPort() {}
GalMultiPort :: ~GalMultiPort() {}

void GenericPort::setAlias (GenericPort& gp) {
	aliasedTo = &gp;
	gp.aliasedFrom = this;
}


PortHole& GenericPort :: newConnection () {
	// my apologies for this horrible cast
	return *(PortHole *)&realPort();
}

// translate aliases, if any.
GenericPort* GenericPort :: translateAliases() {
	GenericPort* p = this;
	while (p->aliasedTo) p = p->aliasedTo;
	return p;
}

// really the same as translateAliases; works on const objects
const GenericPort& GenericPort :: realPort() const {
	const GenericPort* p = this;
	while (p->aliasedTo) p = p->aliasedTo;
	return *p;
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

// destructor: remove myself from the circle but preserve a smaller circle
// of typePortPtrs.  If someone is aliased to me, remove the alias.
// Basic idea: remove all pointers to me before deletion.

GenericPort :: ~GenericPort () {
	if (aliasedFrom) aliasedFrom->aliasedTo = aliasedTo;
	if (aliasedTo) aliasedTo->aliasedFrom = aliasedFrom;
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
	// create Geodesic, wire it up.
	PortHole* realSource = &newConnection();
	Geodesic* geo = realSource->allocateGeodesic();
	geo->setSourcePort(*realSource,numberDelays);
	geo->setDestPort(destination);
	return;
}

// set delay by adjusting geodesic.
void PortHole :: setDelay (int delays) {
	if (myGeodesic)
		myGeodesic->setDelay (delays);
}

// This is not a GenericPort method because the concept of disconnecting
// a multiporthole is ambiguous.  Since fancier geodesics work differently,
// this is virtual -- redefined for some domains.
void PortHole :: disconnect(int delGeo) {
	farSidePort = 0;
	if (!myGeodesic) return;

	if (delGeo) {
		// remove the connection on the geodesic end too
		myGeodesic->disconnect(*this);

		// free up geodesic if it is not persistent
		if (!myGeodesic->isItPersistent()) {
			LOG_DEL; delete myGeodesic;
		}
	}
	myGeodesic = 0;
	return;
}

// Porthole constructor.
PortHole :: PortHole () : myGeodesic(0), farSidePort(0), 
	myPlasma(0), myBuffer(0), myMultiPortHole(0) {}
	
EventHorizon* PortHole :: asEH() { return NULL; }

// Porthole destructor.
PortHole :: ~PortHole() {
	disconnect();
	if (parent())
		parent()->removePort(*this);
	LOG_DEL; delete myBuffer;
}

// small virtual functions for PortHole, GalPort, GalMultiPort

void PortHole :: receiveData () { return;}
void PortHole :: sendData () { return;}

// GalPort constructor
GalPort :: GalPort(GenericPort& a) { GenericPort::setAlias(a);}

// get answer by asking the alias.
int GalPort :: isItInput() const { return alias()->isItInput();}
int GalPort :: isItOutput() const { return alias()->isItOutput();}

// constructor: a GalMultiPort always has an alias
GalMultiPort ::	GalMultiPort(GenericPort& a) { GenericPort::setAlias(a);}

// get answer by asking the alias.
int GalMultiPort :: isItInput() const { return alias()->isItInput();}
int GalMultiPort :: isItOutput() const { return alias()->isItOutput();}

// create a new porthole in a GalMultiPort
PortHole& GalMultiPort :: newPort() {
	PortHole& newAliasPort = ((MultiPortHole*)alias())->newPort();
	LOG_NEW; return installPort(*new GalPort(newAliasPort));
}

// return number of tokens waiting on Geodesic
int PortHole :: numTokens() const { return myGeodesic->size();}

// return the number of initial delays on the Geodesic
int PortHole :: numInitDelays() const {
	return myGeodesic->numInit();
}

// adjust the number of initial delays on the Geodesic
void PortHole :: adjustDelays(int newNumDelay) {
	if (newNumDelay != numInitDelays()) {
		myGeodesic->setDelay(newNumDelay);
		myGeodesic->initialize();
	}
}

void PortHole :: allocateBuffer()
{
	// If there is a buffer, release its memory
	LOG_DEL; delete myBuffer;

	// Allocate new buffer, and fill it with initialized Particles
	LOG_NEW; myBuffer = new CircularBuffer(bufferSize);
        for(int i = myBuffer->size(); i>0; i--) {
                Particle** p = myBuffer->next();
                *p = myPlasma->get();
	}
}

PortHole& PortHole :: setPort(const char* s,
                              Block* parent,
                              DataType t) {
// zero my plasma if my type is being changed.
	if (t != type()) myPlasma = 0;
	GenericPort::setPort (s, parent, t);
	numberTokens = 1;
	bufferSize = numberTokens;
        return *this;
}

// Print a Generic Port
StringList
GenericPort :: print (int) const {
        StringList out;
 
        if(isItInput())
           out = "      Input ";
        else if(isItOutput())
           out = "      Output ";
        
        out << "PortHole: " << fullName();
        
        if(alias() != NULL)
		out << "       Aliased to: " << realPort().fullName();
	return out << "\n";
}

StringList
PortHole :: print(int) const {
	StringList out = GenericPort::print();
	if (alias() == NULL) {
		if (far() != NULL) {
			out << "    Connected to port: "
			    << far()->fullName();
			if (myGeodesic->numInit() > 0)
				out << "(delay = "
				    << myGeodesic->numInit() << ")";
			out += "\n";
		}
		else
			out << "    Not connected.\n";
        }
        return out;
}

// sets the index values of each porthole of each star in the galaxy.
// Returns the total number of portholes.
int setPortIndices(Galaxy& g) {
	GalStarIter nextStar(g);
	Star* s;
	int cnt = 0;
	while ((s = nextStar++) != 0) {
		BlockPortIter nextPort(*s);
		PortHole* p;
		while ((p = nextPort++) != 0)
			p->indexValue = cnt++;
	}
	return cnt;
}

int MultiPortHole :: isItMulti() const { return TRUE;}

void MultiPortHole :: initialize() {}

// MPH constructor
MultiPortHole :: MultiPortHole() : peerMPH(0), busDelay(0) {}

// MPH destructor

MultiPortHole :: ~MultiPortHole() { delPorts();}

// delete and reinitialize the porthole list.  Any bus connection is
// broken as well.
void MultiPortHole :: delPorts () {
	ports.deleteAll();
	if (peerMPH) {
		peerMPH->peerMPH = 0;
		peerMPH = 0;
	}
}

StringList
MultiPortHole :: print (int verbose) const {
	StringList out;
	out << "Multi " << GenericPort::print(verbose);
	if (peerMPH) {
		out += "bus connection to multiporthole ";
		out += peerMPH->fullName();
	}
	out << "This MultiPortHole contains " <<numberPorts()
	    <<" PortHoles.\n";
// We don't add stuff on the PortList, since that will be printed
// out by the enclosing Block::printPorts.
	return out;
}

// define a marker value to prevent infinite recursion
Plasma* const Mark = (Plasma*)1;

// return the type of my Plasma
DataType PortHole :: resolvedType() const {
	return myPlasma ? myPlasma->type() : 0;
}

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

// check for infinite recursion
	if (myPlasma == Mark) return 0;

// Set initial value if not set and not ANYTYPE.
	if (myPlasma == 0 && type() != ANYTYPE)
		myPlasma = Plasma::getPlasma(type());

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
			// mark neighbors that may have problems
			// also, avoid infinite loop in case of corrupted
			// typePort ptrs that don't form a loop
			GenericPort* q = typePort();
			// mark at most 10 stars
			int nMark = 10;
			while (q && q != this && nMark > 0) {
				Error::mark(*q);
				q = q->typePort();
				nMark--;
			}
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
			if (!myPlasma && far()) {
				myPlasma = Mark;
				myPlasma = far()->setPlasma();
			}
		}
	} 
// If it is an output PortHole.
	else {	
		// first, if far() has known type, use it
		if (far() && far()->myPlasma != Mark &&
		    (far()->myPlasma || far()->type() != ANYTYPE)) {
			if (!far()->myPlasma) {
				Plasma* p = Plasma::getPlasma(far()->type());
				far()->myPlasma = p;
			}
			myPlasma = far()->myPlasma;
		}
		// or, far() has typePort: set far() and use it
		else if (far() && far()->typePort()) {
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
	if (!setPlasma ()) {
		Error::abortRun (*this, "can't determine DataType");
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
	if(myGeodesic && (isItOutput() || asEH()))
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
	// use the much faster "here" method if delay = 0
	Particle** p = delay ? myBuffer->previous(delay)
			     : myBuffer->here();
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
                              DataType t) {
	GenericPort::setPort (s, parent, t);
        return *this;
}

// Method to generate names for new portholes.  Names are of the
// form "name#num", where name is the MultiPortHole name and num
// is a sequence number.  The number equals the current number of
// contained ports.
const char*
MultiPortHole :: newName () {
	StringList buf = name();
	buf << "#" << ports.size();
// save the string on the heap.  hashstring avoids multiple copies of "input#1"
	return hashstring (buf);
}

// Add a new PortHole to the MultiPortHole.  install is provided so
// we can do multiports for specific domains.
// This function is a friend of class PortHole.

PortHole& MultiPortHole :: installPort(PortHole& p) {
	ports.put(p);
	parent()->addPort(p.setPort(newName(), parent(), type()));
// for ANYTYPE multiportholes, all ports are resolved to be the same type.
	if (type() == ANYTYPE)
		p.inheritTypeFrom(*this);
	// we can do the following as a friend function
	p.myMultiPortHole = this;
	return p;
}

PortHole& MultiPortHole :: newPort() {
	LOG_NEW; return installPort(*new PortHole);
}

// Return a PortHole for a new connection.  We return the first available
// unconnected PortHole.  If there are none, we make a new one.

PortHole& MultiPortHole :: newConnection() {

	// find an unconnected porthole
	MPHIter next(*this);
	PortHole* p;
	while ((p = next++) != 0) {
		// work right even for GalMultiPortHole
		// note that newConnection, when applied to a
		// PortHole, returns what that PortHole is aliased
		// to.  We cannot just check far() on p since, if
		// it has an alias, its far will be null.
		if (p->newConnection().far() == NULL) return *p;
	}

	// no disconnected ports, make a new one.
	return newPort();
}

// Create a bus connection between two multiportholes.
// any pre-existing connection to either is broken.
// if bus already exists we can efficiently do nothing or widen it.

void MultiPortHole :: busConnect (MultiPortHole& peer, int width, int del) {
	if (peerMPH == &peer && del == busDelay && width >= numberPorts()) {
		// fast way: do nothing or widen existing bus
		for (int i = numberPorts(); i < width; i++)
			connect (peer, del);
		return;
	}
	// slow way: disconnect, build the bus.
	delPorts();
	peer.delPorts();
	for (int i = 0; i < width; i++)
		connect (peer, del);
	busDelay = del;
}

// allocate a new Geodesic.  Set its name and parent according to the
// source porthole (i.e. *this).
Geodesic* PortHole :: allocateGeodesic () {
	char buf[80];
	strcpy (buf, "Node_");
	strcat (buf, name());
	LOG_NEW; Geodesic *g = new Geodesic;
	g->setNameParent(hashstring(buf), parent());
	return g;
}

// additional methods for PortHoles

// Particle movement methods

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
		(*p)->die();
 
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

	if (numberTokens == 1) {
		// fast case for when moving one particle
		// most common case.
		p = myBuffer->here();
		myGeodesic->put(*p);
		*p = myPlasma->get();
		return;
	}
	// slow case for multiple particles.
	// Back up in the buffer by numberTokens
	myBuffer->backup(numberTokens);

	// Now move forward numberTokens times, sending copy of
	// Particle to Geodesic
	for(int i = numberTokens; i>0; i--) {
		// get next particle from my buffer
		p = myBuffer->next();
		
		// launch it into the geodesic
		myGeodesic->put(*p);

		// Get Particle from Plasma
		// Put it my buffer, replacing the used-up particle.
		*p = myPlasma->get();
	}
}

// isa functions
ISA_FUNC(GenericPort,NamedObj);
ISA_FUNC(PortHole,GenericPort);
ISA_FUNC(MultiPortHole,GenericPort);
