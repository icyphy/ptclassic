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
#include "Domain.h"
#include "Wormhole.h"
 
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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
GenericPort :: GenericPort () : myResolvedType(0), myType(ANYTYPE),
              aliasedTo(0), aliasedFrom(0), typePortPtr(0), attributeBits(0) {}

// Small virtual methods
int GenericPort :: isItInput () const { return FALSE;}
int GenericPort :: isItOutput () const { return FALSE;}
int GenericPort :: isItMulti () const { return FALSE;}

GalPort :: ~GalPort() {}
GalMultiPort :: ~GalMultiPort() {}

void GenericPort::setAlias (GenericPort& gp) {
    // We allow inserting an alias within an existing chain.
    // For robustness, we must check if gp.aliasedFrom == this
    // which would create a loop of aliases.  This could happen,
    // if the user called alias twice.
    if (gp.aliasedFrom && gp.aliasedFrom != this) {
	aliasedFrom = gp.aliasedFrom;
	aliasedFrom->aliasedTo = this;
    }
    aliasedTo = &gp;
    gp.aliasedFrom = this;
}

PortHole& GenericPort :: newConnection () {
	// first resolve aliases.
	GenericPort& real = realPort();
	if (!real.isItMulti()) return (PortHole&)real;
	// Alias points to a MPH.  Use its newConnection method.
	return real.newConnection();
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
// Only this function and the destructor alter typePortPtr.
//
// When a MultiPortHole is made part of an inheritance loop,
// we want the MPH's children all to become part of the loop as well.
// Making the children part of the loop is the useful work; the MPH itself
// doesn't need to have a resolved type.  We include the MPH itself in
// the loop just as an easy way to flag that any subsequently-created
// children of the MPH must also be added to the loop (see installPort).

void GenericPort :: inheritTypeFrom(GenericPort& p) {

  if (isItMulti() && typePortPtr == 0) {
    // I am an MPH that has not previously been part of an inheritance chain.
    // Make me and my existing children into a loop that we will add to.
    // We use a recursive call so that the right things happen if some
    // existing child is already part of a chain.
    typePortPtr = this;		// prevent infinite recursion!
    MPHIter next(*(MultiPortHole*) this);
    PortHole* ph;
    while ((ph = next++) != 0) {
      ph->inheritTypeFrom(*this);
    }
  }
  if (p.isItMulti() && p.typePortPtr == 0) {
    // Do the same for the far end.
    p.typePortPtr = &p;		// prevent infinite recursion!
    MPHIter next((MultiPortHole&) p);
    PortHole* ph;
    while ((ph = next++) != 0) {
      ph->inheritTypeFrom(p);
    }
  }

  // If I am already part of a circle, enlarge the circle to include p.
  // As a part of traversing my circle we can check to see if p is already
  // a member; if so, we must do nothing to avoid corrupting the circle.
  GenericPort* temp = typePortPtr;
  if (temp) {
	if (temp == &p) return;		// already linked
        while (temp->typePortPtr != this) {
		temp = temp->typePortPtr;
		if (temp == &p) return;	// already linked
	}
	temp->typePortPtr = &p;
  } else {
	typePortPtr = &p;
  }

  // Likewise, if p is already part of a circle then enlarge it to include me.
  // At this point I am surely not in p's circle, so need not check.
  temp = p.typePortPtr;
  if (temp) {
        while (temp->typePortPtr != &p) {
		temp = temp->typePortPtr;
	}
	temp->typePortPtr = this;
  } else {
	p.typePortPtr = this;
  }
}

// destructor: remove myself from the circle but preserve a smaller circle
// of typePortPtrs.  If someone is aliased to me, remove the alias.
// Basic idea: remove all pointers to me before deletion.

GenericPort :: ~GenericPort () {
        clearAliases();
	if (!typePortPtr) return;
	GenericPort* q = typePortPtr;
	while (q->typePortPtr != this) q = q->typePortPtr;
	q->typePortPtr = typePortPtr;
}

void GenericPort :: clearAliases() {
	if (aliasedFrom) aliasedFrom->aliasedTo = aliasedTo;
	if (aliasedTo) aliasedTo->aliasedFrom = aliasedFrom;
	aliasedFrom = aliasedTo = 0;
}

// The connect method
// This method is virtual and may be overridden
// 3/2/94 changed to add initDelayValues
void GenericPort :: connect(GenericPort& destination, int numberDelays,
			    const char* initDelayValues)
{
	// Resolve any aliases and MultiPortHole stuff:
	// newConnection is a virtual function that does the right
	// thing for all types of PortHoles.
	// create Geodesic, wire it up.
	PortHole& realSource = newConnection();
	PortHole& realDestination = destination.newConnection();
	Geodesic* geo = realSource.allocateGeodesic();
	if (geo == NULL) geo = realDestination.allocateGeodesic();
	if (geo != NULL)
	{
	    if (realSource.isItOutput())	// Yes, it is a source.
	    {
		geo->setSourcePort(realSource, numberDelays, initDelayValues);
		geo->setDestPort(realDestination);
	    }
	    else	// The destination is really the source.
	    {
		geo->setSourcePort(realDestination, numberDelays, initDelayValues);
		geo->setDestPort(realSource);
	    }
	}
	else Error::abortRun(realSource,
		"could not allocate geodesic for connection.");
	return;
}

// set delay by adjusting geodesic.
// 3/2/94 changed to add initDelayValues
void PortHole :: setDelay (int numberDelays, const char* initDelayValues) {
	if (myGeodesic)
		myGeodesic->setDelay(numberDelays, initDelayValues);
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
myPlasma(0), myBuffer(0), myMultiPortHole(0) {}
	
EventHorizon* PortHole :: asEH() { return NULL; }
	
ClusterPort* PortHole :: asClusterPort() { return NULL; }

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
int GalPort :: isItInput() const { 
  if (alias()) return alias()->isItInput();
  else return FALSE;
}
int GalPort :: isItOutput() const {
  if (alias()) return alias()->isItOutput();
  else return FALSE;
}

// constructor: a GalMultiPort always has an alias
GalMultiPort ::	GalMultiPort(GenericPort& a) { GenericPort::setAlias(a);}

// get answer by asking the alias.
int GalMultiPort :: isItInput() const { 
  if (alias()) return alias()->isItInput();
  else return FALSE;
}
int GalMultiPort :: isItOutput() const {
  if (alias()) return alias()->isItOutput();
  else return FALSE;
}

// create a new porthole in a GalMultiPort
PortHole& GalMultiPort :: newPort() {
  if (alias()) {
    PortHole& newAliasPort = alias()->newPort();
    LOG_NEW; return installPort(*new GalPort(newAliasPort));
  } else {
    // FIXME: Memory leak
    // Must get a porthole somehow so we don't dump core.
    Error::abortRun(*this,
		    "Attempt to create a new GalPort when there is no alias");
    LOG_NEW; return installPort(*new GalPort());
  }
}

// return number of tokens waiting on Geodesic
int PortHole :: numTokens() const {
        return myGeodesic?myGeodesic->size():-1;
}

// return the number of initial delays on the Geodesic
int PortHole :: numInitDelays() const {
	return myGeodesic?myGeodesic->numInit():-1;
}

// 3/2/94 added to support initDelayValues
const char* PortHole::initDelayValues() const {
	return myGeodesic?myGeodesic->initDelayValues(): (const char*)NULL;
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
	// make sure that the buffer size is at least 1
	// the dynamic dataflow domain uses nmv=0 to flag dynamic ports
	if ( numberTokens <= 0 ) bufferSize = 1;
	else bufferSize = numberTokens;
        return *this;
}

// Print a Generic Port
StringList GenericPort :: print (int) const {
    StringList out;
 
    if(isItInput())
	out << "\tInput ";
    else if(isItOutput())
	out << "\tOutput ";
        
    out << name();
    
    if(alias() != NULL)
	out << "\t\tAliased to: " << realPort().fullName();

    return out << "\n";
}

StringList PortHole :: print(int) const {
    StringList out = GenericPort::print();
    if (alias() == NULL) {
	if (far() != NULL) {
	    out << "\tConnected to port: " << far()->fullName();
	    if (myGeodesic->numInit() > 0)
		out << "(delay = " << myGeodesic->numInit() << ")";
	    if (myGeodesic->initDelayValues())
		out << "(initValues = "<< myGeodesic->initDelayValues() << ")";
	    // added to visibility into PortHole objects via ptcl interface
	    out << "(numTokens = " << numberTokens << ")";
	}
	else
	    out << "\tot connected.";
    }
    return out << "\n\n";
}

// set the alias and fix the far pointers
void PortHole::setAlias(PortHole& blockPort) {
    GenericPort::setAlias (blockPort);
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
// 3/2/94 changed to add initDelayValues
MultiPortHole :: MultiPortHole() : peerMPH(0), numDelaysBus(0),
                                   initDelayValuesBus(0) {}

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

StringList MultiPortHole :: print (int verbose) const {
    StringList out;
    out << GenericPort::print(verbose);
    if (peerMPH) {
	out << "\tbus connection to multiporthole " << peerMPH->fullName();
    }
    out << "This MultiPortHole contains " << numberPorts() <<" PortHoles.\n\n";
// We don't add stuff on the PortList, since that will be printed
// out by the enclosing Block::printPorts.
    return out;
}

// These routines are concerned with propagating type information around
// the porthole connection structure.  There are several considerations:
// 1) We allow ports with different types to be connected together; the
//    output porthole determines what particle type to use.
// 2) ANYTYPE must be allowed and correctly resolved in three different cases:
//    * Printer and similar polymorphic stars, which accept any input type.
//    * Fork and similar stars, which want to bind multiple outputs to
//	the type of a given input.
//    * Merge and similar stars, which want to resolve the type of a single
//	output to the common type of several inputs, if possible.  (If there
//	is no common input type, we use the type of the destination port,
//	or declare error if the destination port has indeterminate type.)
//    typePort() linkages are used to bind ports together for ANYTYPE
//    resolution.
// 3) We have to recursively propagate type info in order to deal with
//    chains of ANYTYPE stars.
// 4) Wormhole boundaries need special treatment since far() doesn't link
//    to the actual far end porthole, only the near side of the event
//    horizon.  We have to propagate type info all the way through.
//
// In Ptolemy 0.6 and earlier, the members of a multiporthole were always
// constrained to have the same type.  This is no longer true, since it gets
// in the way for polymorphic stars.  But if a multiporthole is tied to
// another porthole via inheritTypeFrom, then each member porthole is
// supposed to match the type of that other porthole.  (Even then,
// inheritTypeFrom is not a guarantee of type equality; we do not complain
// if the resolution algorithm ends up assigning different types.)
//
// Most stars that have ANYTYPE outputs should tie the port type to an input
// porthole to avoid its being undefined.  An exception is HOF-type stars,
// which escape the need to specify a porthole type by removing themselves
// from the graph before porthole type resolution occurs.
//
// In some cases the type is really undefined.  Consider this universe
// (using ptcl syntax)
// 		star f Fork; star p Printer
// 		connect f output f input 1
// 		connect f output p input
// There are no types anywhere in the system.  We have little choice
// but to declare an error.
//
// To give convenient results for both Fork-like and Merge-like stars,
// we resolve types in two passes.
//
// 1. The first pass is "feed forward" from outputs to inputs: any output
// port of defined type is resolved to that type, as is its connected input.
// Then, within any type equivalence loop in which all the input ports are
// resolved to the same type, we resolve unresolved outputs to that type
// (and resolve their connected inputs, leading to recursive propagation).
// This pass handles Fork-like stars as well as Merge stars whose inputs
// all have the same type.
//
// 2. The second pass is "feed back" from inputs to outputs: any unresolved
// input port with a defined type is resolved to that type, as is its
// connected output.  Then, within any type equivalence loop in which all the
// outputs are resolved to the same type, we resolve unresolved inputs to
// that type (and recursively resolve their connected outputs).
// This pass allows us to do something reasonable with Merge stars that have
// inputs of different types: if the merge output is going to a port of
// knowable type, we may as well just output particles of that type.
//
// 3. Finally, we declare error if any porthole types remain unresolved.
// This occurs if a Merge-like star has inputs of nonidentical types and
// an output connected to an ANYTYPE input.  The user must insert type
// conversion stars to coerce the Merge inputs to a common type, so that
// the system can figure out what type to use for the Merge output.

// define a marker value to prevent infinite recursion
const DataType Mark = "MARK";

DataType PortHole :: setResolvedType () {
  if (myResolvedType && myResolvedType != Mark)
    return myResolvedType;	// already determined, just return it

  // The resolver subroutines get confused if started from the inside
  // boundary ghost port of a wormhole, since it has a "backwards"
  // setting of isItInput/isItOutput.  The simplest answer seems to be
  // to make sure we start the recursion from the attached ordinary port.
  if (asEH()) {
    // Even this is a tad tricky, because we have to look far enough to
    // find a non-EH port; there could be an adjacent wormhole boundary.
    // We use settings to Mark to control a recursive outward search.
    if (far() && far()->resolvedType() != Mark) {
      myResolvedType = Mark;
      myResolvedType = far()->setResolvedType();
    }
    if (!myResolvedType) {
      PortHole *ghost = asEH()->ghostAsPort();
      if (ghost && ghost->resolvedType() != Mark) {
	myResolvedType = Mark;
	myResolvedType = ghost->setResolvedType();
      }
    }
  } else {
    // normal case at non-EH port
    if (! resolvePass1())
      resolvePass2();
  }

  if (myResolvedType == Mark)
    return 0;			// shouldn't happen, but...

  return myResolvedType;
}

DataType GenericPort :: resolvePass1 (DataType useType) {
  // If being told what to do, do it.
  if (useType) {
    // In pass 1, only input ports should be told what to do ---
    // this represents propagation of resolved type from connected output.
    // Also, I should never be told to change my type once resolved.
    if (!isItInput() ||
	(myResolvedType && myResolvedType != Mark && myResolvedType != useType))
      Error::abortRun(*this, "internal error in GenericPort::resolvePass1");
    myResolvedType = useType;
  }
  // Resolve output portholes of known types and input portholes that are
  // connected to output portholes of pass1-resolvable types.
  // If I am a member of a type equivalence chain, scan all the chain members.
  GenericPort *p = this;
  do {
    // ignore chain members already resolved or marked, as well as MPH members
    if (p->myResolvedType == 0 && !p->isItMulti()) {
      PortHole& ph = *(PortHole*) p;
      if (ph.isItOutput()) {
	if (ph.type() != ANYTYPE) {
	  // Pass 1 rule 1: resolve defined-type output ports to their type.
	  ph.myResolvedType = ph.type();
	  // Pass 1 rule 2: propagate that type to connected input.
	  PortHole* farend = ph.far();
	  while (farend && farend->asEH()) {
	    // cross through wormhole boundary, fixing EH types as we go.
	    farend->myResolvedType = ph.myResolvedType; // near EH
	    farend = farend->asEH()->ghostAsPort();
	    farend->myResolvedType = ph.myResolvedType; // far EH
	    farend = farend->far(); // now far = true far end port
	  }
	  if (farend)
	    farend->resolvePass1(ph.myResolvedType);
	}
      } else { // input port
	// see if far end has pass1-resolvable type
	PortHole* farend = ph.far();
	while (farend && farend->asEH())
	  farend = farend->asEH()->ghostAsPort()->far();
	if (farend) {
	  ph.myResolvedType = Mark; // set recursion stop
	  ph.myResolvedType = farend->resolvePass1();
	  // note: on successful resolution, the far end will have called me
	  // back and so myResolvedType will be set already.  Assigning the
	  // return value is just to clear the Mark if not successful.
	}
      }
    }
    p = p->typePort();
  } while (p && p != this);
  // Now see if we are ready to apply rule 3: are all the resolvable
  // input members of the equivalence chain resolved to the same type?
  if (typePort()) {
    DataType inputType = 0;
    int haveUnknown = FALSE;
    p = this;
    do {
      if (!p->isItMulti()) {
	PortHole& ph = *(PortHole*) p;
	if (ph.isItOutput()) {
	  if (ph.myResolvedType == 0)
	    haveUnknown = TRUE;	// worth doing the third scan
	} else { // input port
	  if (ph.myResolvedType) { // ignore unresolvable inputs
	    if (ph.myResolvedType == Mark ||
		(inputType && inputType != ph.myResolvedType)) {
	      inputType = 0;	// pass 1 still busy, or not all same type
	      break;
	    }
	    inputType = ph.myResolvedType;
	  }
	}
      }
      p = p->typePort();
    } while (p && p != this);
    if (inputType && haveUnknown) {
      // Yes, resolve the unknown outputs to this type
      p = this;
      do {
	if (!p->isItMulti()) {
	  PortHole& ph = *(PortHole*) p;
	  if (ph.isItOutput() && ph.myResolvedType == 0) {
	    ph.myResolvedType = inputType;
	    // Pass 1 rule 2: propagate that type to connected input.
	    PortHole* farend = ph.far();
	    while (farend && farend->asEH()) {
	      // cross through wormhole boundary, fixing EH types as we go.
	      farend->myResolvedType = ph.myResolvedType; // near EH
	      farend = farend->asEH()->ghostAsPort();
	      farend->myResolvedType = ph.myResolvedType; // far EH
	      farend = farend->far(); // now far = true far end port
	    }
	    if (farend)
	      farend->resolvePass1(ph.myResolvedType);
	  }
	}
	p = p->typePort();
      } while (p && p != this);
    }
  }
  return myResolvedType;
}

DataType GenericPort :: resolvePass2 (DataType useType) {
  // If being told what to do, do it.
  if (useType) {
    // In pass 2, only output ports should be told what to do ---
    // this represents propagation of resolved type from connected input.
    // Also, I should never be told to change my type once resolved.
    if (!isItOutput() ||
	(myResolvedType && myResolvedType != Mark && myResolvedType != useType))
      Error::abortRun(*this, "internal error in GenericPort::resolvePass2");
    myResolvedType = useType;
  }
  // Resolve input portholes of defined types and output portholes that are
  // connected to input portholes of pass2-resolvable types.
  // If I am a member of a type equivalence chain, scan all the chain members.
  GenericPort *p = this;
  do {
    // ignore chain members already resolved or marked, as well as MPH members
    if (p->myResolvedType == 0 && !p->isItMulti()) {
      PortHole& ph = *(PortHole*) p;
      if (ph.isItInput()) {
	if (ph.type() != ANYTYPE) {
	  // Pass 2 rule 1: resolve defined-type input ports to their type.
	  ph.myResolvedType = ph.type();
	  // Pass 2 rule 2: propagate that type to connected output.
	  PortHole* farend = ph.far();
	  while (farend && farend->asEH()) {
	    // cross through wormhole boundary, fixing EH types as we go.
	    farend->myResolvedType = ph.myResolvedType; // near EH
	    farend = farend->asEH()->ghostAsPort();
	    farend->myResolvedType = ph.myResolvedType; // far EH
	    farend = farend->far(); // now far = true far end port
	  }
	  if (farend)
	    farend->resolvePass2(ph.myResolvedType);
	}
      } else { // output port
	// see if far end has pass2-resolvable type
	PortHole* farend = ph.far();
	while (farend && farend->asEH())
	  farend = farend->asEH()->ghostAsPort()->far();
	if (farend) {
	  ph.myResolvedType = Mark; // set recursion stop
	  ph.myResolvedType = farend->resolvePass2();
	}
      }
    }
    p = p->typePort();
  } while (p && p != this);
  // Now see if we are ready to apply rule 3: are all the resolvable
  // output members of the equivalence chain resolved to the same type?
  if (typePort()) {
    DataType outputType = 0;
    int haveUnknown = FALSE;
    p = this;
    do {
      if (!p->isItMulti()) {
	PortHole& ph = *(PortHole*) p;
	if (ph.isItInput()) {
	  if (ph.myResolvedType == 0)
	    haveUnknown = TRUE;	// worth doing the third scan
	} else { // output port
	  if (ph.myResolvedType) { // ignore unresolvable outputs
	    if (ph.myResolvedType == Mark ||
		(outputType && outputType != ph.myResolvedType)) {
	      outputType = 0;	// pass 2 still busy, or not all same type
	      break;
	    }
	    outputType = ph.myResolvedType;
	  }
	}
      }
      p = p->typePort();
    } while (p && p != this);
    if (outputType && haveUnknown) {
      // Yes, resolve the unknown inputs to this type
      p = this;
      do {
	if (!p->isItMulti()) {
	  PortHole& ph = *(PortHole*) p;
	  if (ph.isItInput() && ph.myResolvedType == 0) {
	    ph.myResolvedType = outputType;
	    // Pass 2 rule 2: propagate that type to connected output.
	    PortHole* farend = ph.far();
	    while (farend && farend->asEH()) {
	      // cross through wormhole boundary, fixing EH types as we go.
	      farend->myResolvedType = ph.myResolvedType; // near EH
	      farend = farend->asEH()->ghostAsPort();
	      farend->myResolvedType = ph.myResolvedType; // far EH
	      farend = farend->far(); // now far = true far end port
	    }
	    if (farend)
	      farend->resolvePass2(ph.myResolvedType);
	  }
	}
	p = p->typePort();
      } while (p && p != this);
    }
  }
  return myResolvedType;
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
		// set error mark on parent star
		if (parent())
			Error::mark(*parent());
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
	// input porthole), reset myGeodesic
	if (far() && myGeodesic && (isItOutput() || (!asEH() && atBoundary())))
		myGeodesic->initialize();
}

// Similar to initialize() but still keep its infrastructure.
void PortHole :: resetBufferValues() {
        if (myBuffer) {
	        // initialize buffer
	        for(int i = myBuffer->size(); i>0; i--) {
		         Particle** p = myBuffer->next();
			 if (*p) (*p)->initialize();
			 else     *p = myPlasma->get();
		}
	}

	// If this is an output PortHole (or connected to an
	// input porthole), reset myGeodesic
	if (far() && myGeodesic && (isItOutput() || (!asEH() && atBoundary())))
		myGeodesic->resetBufferValues();
}

Particle& PortHole ::  operator % (int delay)
{
	// use the much faster "here" method if delay = 0
	Particle** p = delay ? myBuffer->previous(delay)
			     : myBuffer->here();
	if(p == NULL || *p == NULL) {
		Error::abortRun(*this,CircularBuffer::errMsg());
		// FIXME: memory leak
		// Must get a particle somehow so we don't dump core.
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

PortHole& MultiPortHole :: installPort(PortHole& p) {
	ports.put(p);
	p.setPort(newName(), parent(), type());
	if (parent()) parent()->addPort(p);
	letMeKnownToChild(p);
	// If I am part of an inheritance chain, add the child to it too.
	if (typePort()) p.inheritTypeFrom(*this);
	return p;
}

// This function is a friend of class PortHole.
void MultiPortHole :: letMeKnownToChild(PortHole& p) {
	p.myMultiPortHole = this;
}

PortHole& MultiPortHole :: newPort() {
	LOG_NEW; return installPort(*new PortHole);
}

// Return a PortHole for a new connection.  We return the first available
// unconnected PortHole.  If there are none, we make a new one.
// NOTE: the result must be suitable for making a connection to;
// hence it must be a non-alias porthole.

PortHole& MultiPortHole :: newConnection() {
	// first resolve aliases.
	GenericPort& real = realPort();
	// check for being aliased to a single porthole (is this possible?)
	if (!real.isItMulti()) return (PortHole&)real;
	MultiPortHole& realmph = (MultiPortHole&)real;
	// find an unconnected porthole in the real port
	MPHIter next(realmph);
	PortHole* p;
	while ((p = next++) != 0) {
		// do the right thing if a member of realmph is an alias.
		// I think this is not really possible, but play it safe.
		PortHole& realp = p->newConnection();
		// realp is disconnected iff it has no far()
		if (realp.far() == NULL) return realp;
		// We cannot just check far() on p since, if
		// it is an alias, its far will be null.
		// And we don't want to return an alias anyway.
	}

	// no disconnected ports, make a new one.
	// NOTE: newPort() on a GalMultiPort returns the new alias porthole,
	// so we have to resolve it to its real porthole.
	return newPort().newConnection();
}

// Create a bus connection between two multiportholes.
// any pre-existing connection to either is broken.
// if bus already exists we can efficiently do nothing or widen it.
// 3/2/94 changed to add initDelayValues
void MultiPortHole::busConnect (MultiPortHole& peer, int width, int numDelays,
				const char* initDelayValues) {
	if (peerMPH == &peer && numDelays == numDelaysBus &&
	    initDelayValues == initDelayValuesBus && width >= numberPorts()) {
		// fast way: do nothing or widen existing bus
		for (int i = numberPorts(); i < width; i++)
			connect (peer, numDelays, initDelayValues);
		return;
	}
	// slow way: disconnect, build the bus.
	delPorts();
	peer.delPorts();
	for (int i = 0; i < width; i++)
		connect (peer, numDelays, initDelayValues);
	numDelaysBus = numDelays;
	initDelayValuesBus = initDelayValues;
}

// Get the domain of the galaxy that the parent star is in.
// Return NULL on error.
Domain* domainOf(const GenericPort& port)
{
    Domain* d = NULL;
    if (port.parent()) {
	if(port.parent()->parent())
	    d = Domain::of(*port.parent()->parent());
	else
	    d = Domain::of(*port.parent());
    }
    return d;
}

// allocate a new Geodesic.  Set its name and parent according to the
// source porthole (i.e. *this).
// Return NULL on error.
Geodesic* PortHole :: allocateGeodesic () {
	StringList nm;
	nm << "Node_" << name();
	Geodesic* g = NULL;
	Domain* d = domainOf(*this);
	if (d != NULL) g = &d->newGeo();
	if (g != NULL) g->setNameParent(hashstring(nm), parent());
	return g;
}

// additional methods for PortHoles

// Particle movement methods

void PortHole :: getParticle()
{
  Particle** pOld;
  Particle* pNew;

  // It is assumed this routine is called before a Star executes...
  // It gets numberTokens Particles from the Geodesic and stores them
  // in the buffer, then setting current time to the last Particle input

  for(int i = numberTokens; i>0; i--)
    {
      // Move the current time pointer in the buffer
      // Get a pointer to the next Particle* in the buffer.
      if (!myBuffer) {
	Error::abortRun(*this,
			"has a NULL myBuffer in PortHole::getParticle()");
	return;
      }
      else {
	pOld = myBuffer->next();
      }

      // Get another Particle from the Geodesic.
      if (!myGeodesic) {
	Error::abortRun(*this,
			"has a NULL myGeodesic in PortHole::getParticle()");
	return;
      }
      else {
	pNew = myGeodesic->get();
      }
		
      // Safety check for NULL pNew.
      if (!pNew) {
	Error::abortRun(*this, "Attempt to read from empty geodesic",
			" in PortHole::getParticle()");
	return;
      }

      // Recycle the old particle by putting it into the Plasma.
      if (!myPlasma) {
	Error::abortRun(*this,
			"has a NULL myPlasma in PortHole::getParticle()");
	return;
      }
      else {
	myPlasma->put(*pOld);
      }

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

  // This method is called after go(); the buffer now contains
  // numberTokens Particles that are to be sent to the output Geodesic.

  if (numberTokens == 1) {
    // fast case for when moving one particle
    // most common case.

    if (!myBuffer) {
      Error::abortRun(*this,
		      "has a NULL myBuffer in PortHole::putParticle()");
      return;
    }
    else {
      p = myBuffer->here();
    }

    if (!myGeodesic) {
      Error::abortRun(*this,
		      "has a NULL myGeodesic in PortHole::putParticle()");
      return;
    }
    else {
      myGeodesic->put(*p);
    }

    if (!myPlasma) {
      Error::abortRun(*this,
		      "has a NULL myPlasma in PortHole::putParticle()");
      return;
    }
    else {
      *p = myPlasma->get();
    }

    return;
  }
  // slow case for multiple particles.
  // Back up in the buffer by numberTokens
  if (!myBuffer) {
    Error::abortRun(*this,
		    "has a NULL myBuffer in PortHole::putParticle()");
    return;
  }
  else {
    myBuffer->backup(numberTokens);
  }
  // Now move forward numberTokens times, sending copy of
  // Particle to Geodesic
  for(int i = numberTokens; i>0; i--) {
    // get next particle from my buffer
    if (!myBuffer) {
      Error::abortRun(*this,
		      "has a NULL myBuffer in PortHole::putParticle()");
      return;
    }
    else {
      p = myBuffer->next();
    }
    // launch it into the geodesic
    if (!myGeodesic) {
      Error::abortRun(*this,
		      "has a NULL myGeodesic in PortHole::putParticle()");
      return;
    }
    else {
      myGeodesic->put(*p);
    }
    // Get Particle from Plasma
    // Put it my buffer, replacing the used-up particle.
    if (!myPlasma) {
      Error::abortRun(*this,
		      "has a NULL myPlasma in PortHole::putParticle()");
      return;
    }
    else {
      *p = myPlasma->get();
    }
  }
}

// isa functions
ISA_FUNC(GenericPort,NamedObj);
ISA_FUNC(PortHole,GenericPort);
ISA_FUNC(MultiPortHole,GenericPort);
