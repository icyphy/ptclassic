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

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY

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
        LOG_DEL; delete [] buffer;
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
	if (!farSidePort) return;
	if (myGeodesic && delGeo) {

		// remove the connection on the geodesic end too
		myGeodesic->disconnect(*this);

		// free up geodesic if it is not persistent
		if (!myGeodesic->isItPersistent()) {
			LOG_DEL; delete myGeodesic;
		}
	}
	// if my peer points to me, (this happens only for persistent geos)
	// disconnect the peer.
	if (farSidePort) {
		if (farSidePort->farSidePort == this)
			farSidePort->farSidePort = 0;
		farSidePort = 0;
	}
	myGeodesic = 0;
	return;
}

// Porthole constructor.
PortHole :: PortHole () : myGeodesic(0), farSidePort(0), 
myPlasma(0), myBuffer(0), myMultiPortHole(0), myResolvedType(0) {}
	
EventHorizon* PortHole :: asEH() { return NULL; }

// Porthole destructor.  We remove connections and Plasma, and also
// remove ourselves from the parent's port list, if any.
PortHole :: ~PortHole() {
	disconnect();
	deletePlasma();
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
                              DataType t,
			      int nmv) {
// zero my plasma if my type is being changed.
	if (t != type()) deletePlasma();
	GenericPort::setPort (s, parent, t);
	numberTokens = nmv;
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

// enable locking on the connecting Geodesic and Plasma.
void PortHole :: enableLocking(const PtGate& master) {
	if (!far() || !myPlasma || myGeodesic->isLockEnabled()) return;
	myGeodesic->makeLock(master);
	myPlasma->makeLock(master);
}

// inverse of above
void PortHole :: disableLocking() {
	if (!far() || !myPlasma) return;
	myGeodesic->delLock();
	myPlasma->delLock();
}

int PortHole :: isLockEnabled() const {
	return myGeodesic && myGeodesic->isLockEnabled();
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
const DataType Mark = "MARK";

// The setResolvedType function's job is to propagate types all around the
// structure.  It is, unfortunately, necessarily complex.  It supports
// several functions:
// 1) The allowance and correct resolution of ANYTYPE (for Fork, Printer,
//    type stars)
// 2) Allowing ports with different types to be connected together; the
//    input porthole determines what type to use
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

DataType
PortHole :: setResolvedType (DataType useType) {

// check for infinite recursion
	if (myResolvedType == Mark) return 0;

// Set initial value if not set and not ANYTYPE.
	if (myResolvedType == 0 && type() != ANYTYPE)
		myResolvedType = type();

// I am allowed to change my type only if I am an output porthole.
// This happens if, say, an output of type FLOAT feeds an input of
// type Complex.
	if (useType) {
		if (useType == myResolvedType) return useType;
		if (!myResolvedType || isItOutput()) {
			myResolvedType = useType;
			// following recursion ends when we get to ourself
			if (typePort()) typePort()->setResolvedType(useType);
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
			return myResolvedType;
		}
	}
// If it is an input PortHole
	else if (isItInput()) {
// If my type isn't known try to set it.  The settings to Mark avoid infinite
// recursion.
		if (!myResolvedType) {
			if (typePort()) {
				myResolvedType = Mark;
				myResolvedType = typePort()->setResolvedType();
			}
			// If still not set, try the connected output
			if (!myResolvedType && far()) {
				myResolvedType = Mark;
				myResolvedType = far()->setResolvedType();
			}
		}
	} 
// If it is an output PortHole.
	else {	
		// first, if far() has known type, use it
		if (far() && far()->myResolvedType != Mark &&
		    (far()->myResolvedType || far()->type() != ANYTYPE)) {
			if (!far()->myResolvedType) {
				DataType p = far()->type();
				far()->myResolvedType = p;
			}
			myResolvedType = far()->myResolvedType;
		}
		// or, far() has typePort: set far() and use it
		else if (far() && far()->typePort()) {
			DataType save = myResolvedType;
			myResolvedType = Mark;
			myResolvedType = far()->setResolvedType();
			if (!myResolvedType) myResolvedType = save;
			if (myResolvedType) far()->setResolvedType(myResolvedType);
		}
		// still not set: look for typePort
		if (myResolvedType == 0 && typePort()) {
			myResolvedType = Mark;
			myResolvedType = typePort()->setResolvedType();
		}
	}
	if (myResolvedType) {
		if (typePort()) typePort()->setResolvedType(myResolvedType);
	}
	return myResolvedType;
}

// Function to get plasma type for a MultiPortHole.
DataType
MultiPortHole :: setResolvedType (DataType useType) {
	return typePort() ? typePort()->setResolvedType(useType) : 0;
}

// by default, use the global Plasma appropriate to my resolved type.
int PortHole :: allocatePlasma() {
	myPlasma = Plasma::getPlasma(myResolvedType);
	if (!myPlasma) {
		Error::abortRun(*this, "PortHole::allocatePlasma failed");
	}
	return (myPlasma != 0);
}

// this function verifies that a Plasma is local and of the given
// type.  If it does, it returns a pointer to it, otherwise it returns 0.
static Plasma* rightLocPlas(Plasma* myPlasma,DataType t) {
	return (myPlasma && myPlasma->isLocal() && myPlasma->type() == t)
		? myPlasma : 0;
}

// remove a reference to my Plasma and delete it if the refcount drops
// to zero.

void PortHole :: deletePlasma() {
	if (myPlasma && myPlasma->decCount() == 0) {
		LOG_DEL; delete myPlasma;
	}
	myPlasma = 0;
}

// alternate function: use a local Plasma for the connection.  This
// function may be used as an overload for allocatePlasma by derived
// PortHole classes.
int PortHole :: allocateLocalPlasma() {
	if (rightLocPlas(myPlasma,myResolvedType)) return TRUE;
	deletePlasma();
	// if other side of connection has a correct local plasma, attach
	// to it and return success.
	if (farSidePort) {
		myPlasma = rightLocPlas(farSidePort->myPlasma,myResolvedType);
		if (myPlasma) {
			myPlasma->incCount();
			return TRUE;
		}
	}
	// just make a Plasma
	myPlasma = Plasma::makeNew(myResolvedType);
	if (myPlasma) {
		myPlasma->incCount();
		return TRUE;
	}
	else return FALSE;
}

void PortHole :: initialize()
{
	if (!setResolvedType ()) {
		Error::abortRun (*this, "can't determine DataType");
		return;
	}
	if (!allocatePlasma()) return;
	// allocate buffer if not allocated or wrong size
	if (myBuffer == NULL || bufferSize != myBuffer->size())
		allocateBuffer ();

	// initialize buffer
	for(int i = myBuffer->size(); i>0; i--) {
		Particle** p = myBuffer->next();
		// Initialize particles on the buffer, so when we
		// restart they do not contain old data
		// fill in any empty holes (which could be caused by
		// errors of some kinds in previous runs).
		if (*p)	(*p)->initialize();
		else *p = myPlasma->get();
	}
	// If this is an output PortHole (or connected to an
	// input porthole), initialize myGeodesic
	if (far() && myGeodesic && (isItOutput() || (!asEH() && atBoundary())))
		myGeodesic->initialize();
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
	// first resolve aliases.
	MultiPortHole& real = realPort();
	// find an unconnected porthole in the real port
	MPHIter next(real);
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
	return real.newPort();
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
		// Get a pointer to the next Particle* in the buffer.
		Particle** pOld = myBuffer->next();

		// Get another Particle from the Geodesic.
		Particle* pNew = myGeodesic->get();
		
		if (!pNew) {
			Error::abortRun(*this,
					"Attempt to read from empty geodesic");
			return;
		}
		// Recycle the old particle by putting it into the Plasma.
		myPlasma->put(*pOld);
		// Put the new particle into the buffer.
		*pOld = pNew;
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
