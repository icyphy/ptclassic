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
GenericPort :: GenericPort () : myType(ANYTYPE),
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
	if (myGeodesic && delGeo) {

		// remove the connection on the geodesic end too
		myGeodesic->disconnect(*this);

		// free up geodesic if it is not persistent
		if (!myGeodesic->isItPersistent()) {
			LOG_DEL; delete myGeodesic;
		}
	}
	myGeodesic = 0;
	// if my peer still points to me, disconnect the peer.
	// (this happens only for persistent geos.)
	if (farSidePort) {
		if (farSidePort->farSidePort == this) {
			// NOTE: order is critical here so as not to crash
			// if farSidePort points to myself ... see DDFSelf.
			farSidePort->myResolvedType = 0;
			farSidePort->myPreferredType = 0;
			farSidePort->farSidePort = 0;
		}
		farSidePort = 0;
	}
	// reset any resolved type determined from the old peer,
	// so that we can redo type resolution with the new one.
	myResolvedType = 0;
	myPreferredType = 0;
}

// Porthole constructor.
PortHole :: PortHole () : myGeodesic(0), farSidePort(0), 
myPlasma(0), myBuffer(0), myResolvedType(0), myPreferredType(0),
myMultiPortHole(0) {}
	
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
	// also reset type resolution
	if (t != type()) {
		deletePlasma();
		myResolvedType = 0;
		myPreferredType = 0;
	}
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
//    input porthole determines what particle type to use.
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
// another porthole via inheritTypeFrom, then each member porthole will
// match the type of that other porthole.  (Even then, inheritTypeFrom
// is not an absolute guarantee of type equality; see below.)
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
// Resolution of ANYTYPE portholes is a fairly tricky process; "correct"
// behavior is not easy to define.  To give convenient behavior in as many
// situations as possible, we adopt the following algorithm.
//
// We operate in two stages: the first selects a "preferred" type for each
// porthole, the second the actual "resolved" type.  The resolved type is
// the actual particle type that will propagate across the geodesic connecting
// two portholes, so it must always agree at both ends of a connection.
// (We retain the preferred type for the use of certain domain-specific
// transformations, as will be explained below.)
//
// Preferred type selection is concerned only with assigning real datatypes
// to ANYTYPE portholes.  The preferred type of a non-ANYTYPE porthole is the
// same as its declared type.  ANYTYPE portholes are handled in two recursive
// passes over the connection graph:
//
// 1. The first pass is "feed forward" from outputs to inputs.  If an ANYTYPE
// porthole is not a member of an inheritance group, but is an input porthole
// and is connected to a porthole of pass-1-assignable type, that porthole's
// type becomes its preferred type.  If it is a member of an inheritance
// group, and all the assignable input portholes of the group receive the
// same type from their connected outputs, then that common type becomes the
// preferred type of all the members of the group.
// This pass handles Fork-like stars as well as Merge stars whose inputs
// all have the same type.
//
// 2. The second pass is "feed back" from inputs to outputs; it is the dual
// of the above.  If an unassigned ANYTYPE porthole is not a member of an
// inheritance group, but is an output porthole and is connected to a porthole
// of pass-2-assignable type, that porthole's type becomes its preferred type.
// If it is a member of an inheritance group, and all the assignable output
// portholes of the group receive the same type from their connected inputs,
// then that common type becomes the preferred type of all the members of the
// group.
// This pass allows us to do something reasonable with Merge stars that have
// inputs of different types: if the merge output is going to a port of
// knowable type, we may as well just output particles of that type.
//
// Finally, we declare error if any porthole types remain unassigned.
// This occurs if a Merge-like star has inputs of nonidentical types and
// an output connected to an (unresolvable) ANYTYPE input.  The user must
// insert type conversion stars to coerce the Merge inputs to a common type,
// so that the system can figure out what type to use for the Merge output.
//
// Resolved type selection is performed after preferred types have been
// determined.  It is fairly simple: an input porthole's resolved type
// is the same as its preferred type, while an output porthole's resolved
// type is that of its connected input.
//
// This rather baroque algorithm does have some simple properties:
// * An input port of a specific declared type will always have that type
//   as its preferred and resolved types.  A star writer can thus assume
//   that the received particles are of the type specified; only stars
//   that declare inputs as ANYTYPE need cope with multiple input types.
//   (However, output portholes may be assigned a type different from what
//   is specified, leading to run-time type conversions.)
// * All members of an inheritance loop are guaranteed to be assigned the
//   same preferred type; an error will be reported if this is not possible.
//
// In some domains it is important that members of an inheritance loop
// have the same resolved type, not just the same preferred type.  (For
// example, the CGC Fork star fails if this is not so, because its various
// portholes are all just aliases for a single variable.)  The domain can
// check this by seeing whether preferred type = resolved type for all
// portholes.  If the types are not the same, it can either report an error
// or splice in a type-conversion star to make them the same.
//
// FIXME: would it be better for strict inheritance type equality to be
// enforced on a per-star basis, rather than a per-domain basis??


// Define a marker value to prevent infinite recursion.
// myPreferredType is temporarily set to Mark to indicate that
// some outer level of recursion is already working on the porthole.
const DataType Mark = "MARK";


DataType PortHole :: setPreferredType () {
  // Fast path if type is already determined
  if (myPreferredType) {
    // if we are revisiting a node that an outer level of recursion is
    // already busy with, return 0 to indicate "not yet resolvable".
    if (myPreferredType == Mark)
      return 0;
    return myPreferredType;
  }

  // We will get confused if started at the inside boundary ghost port of a
  // wormhole, since it has a "backwards" setting of isItInput/isItOutput.
  // setResolvedType() is supposed to take care of this and not call
  // setPreferredType() at an event horizon.  But we'll just double-check.
  if (asEH()) {
    Error::abortRun(*this,
		    "PortHole::setPreferredType invoked at event horizon");
    return 0;
  }

  // Do the main recursive algorithm.
  // Pass 2 is only invoked if we can't determine the type in pass 1.
  if (! assignPass1())
    assignPass2();

  if (myPreferredType == Mark)
    return 0;			// shouldn't happen, but...

  return myPreferredType;
}

// First pass of preferred type assignment.
DataType PortHole :: assignPass1 () {
  // Fast path if type is already determined
  if (myPreferredType) {
    // if we are revisiting a node that an outer level of recursion is
    // already busy with, return 0 to indicate "not yet resolvable".
    if (myPreferredType == Mark)
      return 0;
    return myPreferredType;
  }

  if (typePort() == 0) {
    // Not a member of an inheritance group.
    // If I have a declared type, that is my preferred type.
    if (type() != ANYTYPE) {
      myPreferredType = type();
    } else {
      // Only input portholes are assignable in pass 1.
      if (! isItOutput()) {
	// I am an input porthole: get preferred type from connected output.
	PortHole* farend = findFarEnd();
	if (farend) {
	  myPreferredType = Mark;
	  myPreferredType = farend->assignPass1();
	}
      }
    }
  } else {
    // I am a member of an inheritance group.
    // Walk the loop and see whether any members have defined types.
    // If so, we trivially inherit from them (they'd better all match, too).
    // A side effect of this first loop is to set all the group members to
    // Mark as a recursion stop.
    DataType assignedType = 0;
    GenericPort *p = this;
    do {
      if (p->type() != ANYTYPE) {
	if (assignedType == 0)
	  assignedType = p->type();
	else if (p->type() != assignedType) {
	  Error::abortRun(*this,
		"conflicting porthole types declared in inheritance group");
	}
      }
      if (!p->isItMulti()) {
	PortHole& ph = *(PortHole*) p;
	ph.myPreferredType = Mark;
      }
      p = p->typePort();
    } while (p && p != this);

    if (assignedType == 0) {
      // Normal case: all members of inheritance loop are declared ANYTYPE.
      // Check the connected output of each input port in the loop.
      DataType connectedType = 0;
      int uniqueType = TRUE;
      p = this;
      do {
	if (!p->isItMulti() && !p->isItOutput()) {
	  PortHole& ph = *(PortHole*) p;
	  PortHole* farend = ph.findFarEnd();
	  if (farend) {
	    DataType thisType = farend->assignPass1();
	    if (thisType) {
	      if (connectedType == 0)
		connectedType = thisType;
	      else if (connectedType != thisType)
		uniqueType = FALSE;
	    }
	  }
	}
	p = p->typePort();
      } while (p && p != this);
      // If all the resolvable connections report the same type, assign it.
      if (connectedType && uniqueType)
	assignedType = connectedType;
    }

    // Clean up by setting all members of group to assignedType
    // (which will be 0 if we've failed to select a type).
    p = this;
    do {
      if (!p->isItMulti()) {
	PortHole& ph = *(PortHole*) p;
	ph.myPreferredType = assignedType;
      }
      p = p->typePort();
    } while (p && p != this);
  }

  return myPreferredType;
}

// Second pass of preferred type assignment.
DataType PortHole :: assignPass2 () {
  // We should not get here if the type is already resolved or resolvable.
  if (myPreferredType || type() != ANYTYPE) {
    Error::abortRun(*this, "internal error in PortHole::assignPass2");
    return 0;
  }

  if (typePort() == 0) {
    // Not a member of an inheritance group.
    // Get preferred type from connected porthole.
    PortHole* farend = findFarEnd();
    if (farend) {
      myPreferredType = Mark;
      myPreferredType = farend->setPreferredType();
    }
  } else {
    // I am a member of an inheritance group.
    // Walk the loop and set types to Mark as a recursion stop.
    // We need not check for non-ANYTYPE portholes since assignPass1 did that.
    GenericPort *p = this;
    do {
      if (!p->isItMulti()) {
	PortHole& ph = *(PortHole*) p;
	ph.myPreferredType = Mark;
      }
      p = p->typePort();
    } while (p && p != this);

    // Check the connected input of each output port in the loop.
    DataType connectedType = 0;
    int uniqueType = TRUE;
    p = this;
    do {
      if (!p->isItMulti() && p->isItOutput()) {
	PortHole& ph = *(PortHole*) p;
	PortHole* farend = ph.findFarEnd();
	if (farend) {
	  DataType thisType = farend->setPreferredType();
	  if (thisType) {
	    if (connectedType == 0)
	      connectedType = thisType;
	    else if (connectedType != thisType)
	      uniqueType = FALSE;
	  }
	}
      }
      p = p->typePort();
    } while (p && p != this);
    // If all the resolvable connections report the same type, assign it.
    DataType assignedType = 0;
    if (connectedType && uniqueType)
      assignedType = connectedType;

    // Clean up by setting all members of group to assignedType
    // (which will be 0 if we've failed to select a type).
    p = this;
    do {
      if (!p->isItMulti()) {
	PortHole& ph = *(PortHole*) p;
	ph.myPreferredType = assignedType;
      }
      p = p->typePort();
    } while (p && p != this);
  }

  return myPreferredType;
}

// Final phase of type resolution: assign resolved type given preferred types.
// Note: this is virtual, so it could be overridden in a specific domain.
DataType PortHole :: setResolvedType () {
  // Fast path if type is already determined
  if (myResolvedType) {
    if (myResolvedType == Mark)
      return 0;			// shouldn't happen, but...
    return myResolvedType;
  }

  // We will get confused if started at the inside boundary ghost port of a
  // wormhole, since it has a "backwards" setting of isItInput/isItOutput.
  // The simplest answer seems to be to make sure we start the recursion
  // from the attached ordinary port.
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
    // normal case at non-EH port: first do preferred-type assignment
    if (setPreferredType()) {
      // find the far end and let it do preferred-type assignment too
      PortHole* farend = findFarEnd();
      if (farend)
	farend->setPreferredType();
      if (isItInput()) {
	// I am an input porthole: select my preferred type as resolved type.
	myResolvedType = myPreferredType;
	// If the other end is also an input, OK only if same type.
	// Otherwise we have a conflict with neither side taking precedence.
	if (farend && farend->isItInput()) {
	  if (myResolvedType != farend->myPreferredType)
	    myResolvedType = 0;
	}
      } else {
	// I am an output porthole: get resolved type from connected input.
	if (farend) {
	  myResolvedType = farend->myPreferredType;
	  // If the other end is also an output, OK only if same type.
	  // Otherwise we have a conflict with neither side taking precedence.
	  if (! farend->isItInput()) {
	    if (myResolvedType != myPreferredType)
	      myResolvedType = 0;
	  }
	} else {
	  // A disconnected output resolves to its own preferred type.
	  // (Lack of connection may be an error,
	  // but it's not my place to decide that.)
	  myResolvedType = myPreferredType;
	}
      }
      // Now propagate the resolved type to the partner port and to
      // any intervening event horizons.
      // This propagation is not absolutely necessary, but it can save a few
      // cycles by eliminating future calls from those portholes back to me.
      // Note: depending on call order, we may end up doing this propagation
      // from both ends.  This is OK because the logic above is certain to
      // choose the same resolved type at either end.
      if (farend)
	farend->myResolvedType = myResolvedType;
      farend = far();
      while (farend && farend->asEH()) {
	// We set preferred type = resolved type in the intervening EHs
	// (we assume they are declared ANYTYPE so this is OK).
	farend->myPreferredType = myResolvedType;	// near EH
	farend->myResolvedType = myResolvedType;
	farend = farend->asEH()->ghostAsPort();
	if (farend) {
	  farend->myPreferredType = myResolvedType;	// far EH
	  farend->myResolvedType = myResolvedType;
	  farend = farend->far();
	}
      }
    }
  }

  if (myResolvedType == Mark)
    return 0;			// shouldn't happen, but...

  return myResolvedType;
}


// Handy subroutine for type assignment:
// Find the "true" far end, ignoring any intervening event horizons.
PortHole* PortHole :: findFarEnd() const {
	PortHole* farend = far();
	while (farend && farend->asEH()) {
		farend = farend->asEH()->ghostAsPort();
		if (farend)
			farend = farend->far();
	}
	return farend;
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
