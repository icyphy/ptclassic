/******************************************************************
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

 Programmer:  J. T. Buck
 Date of creation: 3/18/90

 InterpGalaxy is a galaxy that may dynamically be added to, as well
 as cloned.

*******************************************************************/
#ifndef _InterpGalaxy_h
#define _InterpGalaxy_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "DynamicGalaxy.h"
#include "StringList.h"

class Target;

// class for a list of Geodesics (or nodes)
class NodeList : private NamedObjList
{
	friend class NodeListIter;
public:
	NodeList() {}
	~NodeList() { deleteAll();}
	void put(Geodesic& g);
	Geodesic* nodeWithName (const char* name) {
		return (Geodesic*) NamedObjList::objWithName(name);
	}
	int remove(Geodesic* g);
	// pass along baseclass methods.
	NamedObjList::size;
	NamedObjList::deleteAll;
};

class NodeListIter : private NamedObjListIter {
public:
	NodeListIter(NodeList& n) : NamedObjListIter(n) {}
	Geodesic* next() { return (Geodesic*) NamedObjListIter::next();}
	Geodesic* operator++() { return next();}
	NamedObjListIter::reset;
};

class InterpGalaxy: public DynamicGalaxy {
public:
// constructor: first arg sets the class name.  Second specifies the domain.
	InterpGalaxy(const char* c, const char* dom);

// copy constructor: duplicates an existing galaxy
	InterpGalaxy(const InterpGalaxy& g) { copy(g);}

// read the class name
	const char* className() const {return myClassName; }

// assignment operator: change this galaxy to look like another one
	InterpGalaxy& operator=(const InterpGalaxy& g) {
		zero();
		copy(g);
		return *this;
	}

// set the descriptor
	void
	setDescriptor(const char* dtext) { NamedObj::setDescriptor(dtext);}

// initialize: does variable-parameter connections
	void initialize();

// add a new star to the galaxy
	int
	addStar(const char* starname, const char* starclass);

// delete a star from the galaxy
	int
	delStar(const char* starname);

// add a connection (point-to-point)
	int
	connect(const char* srcstar, const char* srcport, const char* dststar,
		const char* dstport, const char* delay = 0);

// add a bus connection
	int
	busConnect(const char* srcstar, const char* srcport,
		   const char* dststar, const char* dstport,
		   const char* width, const char* delay = 0);

// disconnect a porthole (works for point-to-point or netlist connections)
	int
	disconnect(const char* star, const char* port);

// add an alias
	int
	alias(const char* galport, const char* star, const char *starport);

// add a node
	int
	addNode(const char* nodename);

// delete a node
	int
	delNode(const char* nodename);

// connnect a porthole to a node
	int
	nodeConnect(const char* star, const char* port, const char* node,
		    const char* delay = 0);

// add a state
	int
        addState(const char* statename, const char* stateclass, const char* statevalue);

// set a state
	int
        setState(const char* blockname, const char* statename, const char* statevalue);

/// make portholes with a multiporthole
	int
	numPorts(const char* starname, const char* portname, int numP);
	
// change the domain
	int
	setDomain(const char* newDomain);
		
// Make a new, identical galaxy
	Block *makeNew() const;
	Block *clone() const;

// "register" the galaxy (add it to the known list)
	void addToKnownList(const char* domain,Target* t = 0);

// return a contained star/galaxy (this is protected in Galaxy)
	Block* blockWithName(const char* s) {return Galaxy::blockWithName(s);}

// This method lets us get at lower-level galaxies
	Block* blockWithDottedName(const char* s);

// class identification
	int isA(const char*) const;

// Destructor
	~InterpGalaxy();

protected:
	// find a star porthole
	PortHole* findPortHole(const char* star,const char* port);

	// find a star porthole or multiporthole
	// The second of these resolves aliases
	GenericPort* findGenPort(const char* star,const char* port);
	GenericPort* findGenericPort(const char* star,const char* port);

	// find a star multiporthole
	MultiPortHole* findMPH(const char* star, const char* port);

	// routine to reset all structures (like body of destructor)
	void zero();

	// body of copy constructor
	void copy(const InterpGalaxy&);

private:
	StringList actionList;	// saves actions used in building galaxy
	StringList initList;	// actions to be done by initialize()
	NodeList nodes;
	const char* myClassName;
	const char* innerDomain;
};
#endif
