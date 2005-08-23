static const char file_id[] = "InterpGalaxy.cc";
/****************************************************************** 

Version identification:
@(#)InterpGalaxy.cc	2.53 11/13/97

Copyright (c) 1990-1997 The Regents of the University of California.
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

 Programmer:  J. T. Buck
 Date of creation: 3/18/90
 Revisions:
	5/26/90 - I. Kuroda
		add InterpGalaxy::addState and InterpGalaxy::setState

 InterpGalaxy is a galaxy that may dynamically be added to, as well
 as cloned.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "InterpGalaxy.h"
#include "StringList.h"
#include "Error.h"
#include "PortHole.h"
#include "Geodesic.h"
#include "miscFuncs.h"
#include "Domain.h"
#include "Plasma.h"
#include "KnownBlock.h"
#include "KnownState.h"
#include "IntState.h"
#include <ctype.h>

// dummy empty delay value, added 3/2/94
const char emptyDelay = '\0';

// constructor: sets the class name and domain.
InterpGalaxy::InterpGalaxy(const char* c, const char* dom) {
	setDescriptor("An interpreted galaxy");
	if (c) 	myClassName = c;
	setDomain(dom);
	defSource = NULL;
}

// Report an error: no such star or porthole

static void noInstance(const char* star,const char* gal) {
	StringList msg;
	msg = "No instance of ";
	msg += star;
	msg += " in ";
	msg += gal;
	msg += "\n";
	Error::abortRun (msg);
}

// Find a port or multiport without resolving aliases
GenericPort *
InterpGalaxy::findGenPort (const char* star,const char* port) {
	Block *st = blockWithName(star);
	if (st == NULL) {
		noInstance (star, name());
		return 0;
	}
	GenericPort *ph = st->multiPortWithName(port);
	if (ph) return ph;
	ph = st->genPortWithName(port);
	if (ph == NULL) noInstance (port, star);
	return ph;
}

// Find a port or multiport -- preserve its "identity" -- things
// can call isItMulti on the result, say
GenericPort *
InterpGalaxy::findGenericPort (const char* star,const char* port) {
	Block *st = blockWithName(star);
	if (st == NULL) {
		noInstance (star, name());
		return 0;
	}
	GenericPort *ph = st->multiPortWithName(port);
	if (ph) return ph;
	ph = st->portWithName(port);
	if (ph == NULL) noInstance (port, star);
	return ph;
}

// Find a port.  If it's really a multiport this ends up returning
// a port within the multiport.
PortHole *
InterpGalaxy::findPortHole (const char* star,const char* port) {
	Block *st = blockWithName(star);
	if (st == NULL) {
		noInstance (star, name());
		return 0;
	}
	PortHole *ph = st->portWithName(port);
	if (ph == NULL) noInstance (port, star);
	return ph;
}

// Find a port without reporting errors.
// Return zero if the port does not exist.
PortHole *
InterpGalaxy::findPortHoleNE (const char* star,const char* port) {
	Block *st = blockWithName(star);
	if (st == NULL) return 0;
	PortHole *ph = st->portWithName(port);
	return ph;
}

// Find a MPH.
MultiPortHole *
InterpGalaxy::findMPH(const char* star, const char* port) {
	Block *st = blockWithName(star);
	if (st == NULL) {
		noInstance (star, name());
		return 0;
	}
	MultiPortHole *ph = st->multiPortWithName(port);
	if (ph == NULL) noInstance (port, star);
	return ph;
}

// Find a MPH without reporting errors.
// Returns zero if it does not exist.

MultiPortHole *
InterpGalaxy::findMPHNE(const char* star, const char* port) {
	Block *st = blockWithName(star);
	if (st == NULL) return 0;
	MultiPortHole *ph = st->multiPortWithName(port);
	return ph;
}

// Evaluate an expression as if it were an IntState initializer in
// a containing star.  Empty expressions return 0.
static int evalExp(Galaxy* gal, const char* exp, const char* dummyName) {
	if (exp == 0 || *exp == 0) return 0;
	Block temp("", gal, "");
	IntState tstate;
	tstate.setState(dummyName, &temp, exp);
	tstate.initialize();
	return int(tstate);
}

// Return TRUE if the given expression is variable, FALSE if it is a
// constant.  We want to err in the conservative direction, i.e. reporting
// variable falsely is safe, and the other way is not.
static int isVarExp(const char* exp) {
	if (!exp) return 0;	// null is fixed
	while (*exp && (isdigit(*exp) || isspace(*exp))) exp++;
	// return false (not variable) if at end of string, otherwise true.
	return (*exp != 0);
}

/* FIX ME!  If the initial delay values are for non-initializable
   delays (indicated by a leading '*' character) and the expression for
   the number of delays is constant (does not depend on any Galaxy
   parameters) then parse the string *AND CLEAR IT*.  Clearing the string
   prevents connections from being logged on the initList.
*/
static int getNumDelays(Galaxy* gal, const char*& values)
{
    int numDelays = 0;
    if (values && *values == '*' && !isVarExp(values+1))
    {
	numDelays = evalExp(gal,values+1,"delay");
	values = &emptyDelay;
    }
    return numDelays;
}

static void
logConnect(StringList& list,const char* srcStar,const char* srcPipe,
	   const char* dstStar,const char* dstPipe, 
	   const char* initDelayValues) {
	list += "C";
	list += srcStar;
	list += srcPipe;
	list += dstStar;
	list += dstPipe;
	list += initDelayValues;
}

void InterpGalaxy::registerInit(const char*  tag,
				GenericPort* srcPort,
				const char*  initDelayValues,
				GenericPort* dstPort,
				const char*  node,
				const char*  busWidth) {
  // Register a repeatable action on the initList.
  // Source and destination ports may have had aliases resolved,
  // so we need to use getPortHoleName to find out what to call them.
  const char* srcBlockName;
  const char* srcPortName;
  if (! getPortHoleName(*srcPort, srcBlockName, srcPortName)) {
    Error::abortRun (*srcPort, "can't find name in surrounding galaxy");
    return;
  }
  initList += tag;
  initList += srcBlockName;
  initList += srcPortName;
  if (dstPort) {
    const char* dstBlockName;
    const char* dstPortName;
    if (! getPortHoleName(*dstPort, dstBlockName, dstPortName)) {
      Error::abortRun (*dstPort, "can't find name in surrounding galaxy");
      return;
    }
    initList += dstBlockName;
    initList += dstPortName;
  }
  if (busWidth) initList += busWidth;
  if (node)     initList += node;
  initList += initDelayValues;
}


// Given a porthole, determine the block name and porthole name that name it
// in this galaxy.  This is nontrivial since the desired name may be for an
// alias leading to the given porthole.  Returns TRUE if successful.
int InterpGalaxy::getPortHoleName (GenericPort& port,
				   const char* & blockName,
				   const char* & portName)
{
  GenericPort* ph = &port;
  do {
    // Does port belong to a member star of this galaxy?
    Block* b = ph->parent();
    if (b && b->parent() == (Block*) this) {
      blockName = b->name();
      portName = ph->name();
      return TRUE;
    }
    // Nope, try the next alias.
    ph = ph->aliasFrom();
  } while (ph);
  return FALSE;			// failed to find it
}


// Form a point-to-point connection.
int
InterpGalaxy::connect(const char* srcStar,const char* srcPipe,
		      const char* dstStar,const char* dstPipe,
		      const char* initDelayValues) {
	if ((initDelayValues == 0) || (*initDelayValues == 0))
	    initDelayValues = &emptyDelay;
// Get the source and destination ports; do first to verify the names are OK
	PortHole *srcP = findPortHole (srcStar, srcPipe);
	PortHole *dstP = findPortHole (dstStar, dstPipe);
	if (srcP == NULL || dstP == NULL) return FALSE;

// Add the connection to the action list, using original forms of parameters.
	logConnect(actionList,srcStar,srcPipe,dstStar,dstPipe,initDelayValues);

// Check for an old-style delay spec (constant number of delays, no values).
// CAUTION: this can change initDelayValues!
	int numDelays = getNumDelays(this, initDelayValues);

// If the delay spec is variable, add it to the initList so that the
// delays get reinitialized at preinit time.
	if (*initDelayValues)
	  registerInit("C", srcP, initDelayValues, dstP);

// Finally make the connection itself.
	Galaxy::connect (*srcP, *dstP, numDelays, initDelayValues);
	return TRUE;
}

static void
logBus(StringList& list, const char* srcStar,const char* srcPipe,
       const char* dstStar,const char* dstPipe, const char* width,
       const char* initDelayValues) {
	list += "B";
	list += srcStar;
	list += srcPipe;
	list += dstStar;
	list += dstPipe;
	list += width;
	list += initDelayValues;
}

// Form a bus connection
int
InterpGalaxy::busConnect(const char* srcStar,const char* srcPipe,
			 const char* dstStar,const char* dstPipe,
			 const char* width, const char* initDelayValues) {
  if ((initDelayValues == 0) || (*initDelayValues == 0))
    initDelayValues = &emptyDelay;
  MultiPortHole* s = findMPH (srcStar, srcPipe);
  MultiPortHole* d = findMPH (dstStar, dstPipe);
  if (s == 0 || d == 0) return FALSE;

// Add the connection to the action list, using original forms of parameters.
  logBus(actionList,srcStar,srcPipe,dstStar,dstPipe,width,initDelayValues);

// Check for an old-style delay spec (constant number of delays, no values).
// CAUTION: this can change initDelayValues!
  int numDelays = getNumDelays(this, initDelayValues);

// If the delay or width is variable, add it to the initList to be
// reinitialized at preinit time.
  if (isVarExp(width) || *initDelayValues)
    registerInit("B", s, initDelayValues, d, NULL, width);

// And do the connection.
  int w = evalExp(this,width,"busWidth");
  if (w == 0) w = 1;
  s->busConnect (*d, w, numDelays, initDelayValues);
  return TRUE;
}

// Add a star to the galaxy.
int
InterpGalaxy::addStar(const char* starname,const char* starclass) {
	Block *src = KnownBlock::clone(starclass,domain());
	if (src == 0) return FALSE;
	addBlock(src->setBlock(hashstring(starname),this));
//add action to list
	actionList += "S";
	actionList += starname;
	actionList += starclass;
	return TRUE;
}

// Remove a star from the galaxy
int
InterpGalaxy::delStar(const char* starname) {
// the following is a severe violation of information hiding, but
// better this than a core dump
	if (strncmp (starname, "auto-fork", 9) == 0) {
		Error::abortRun (*this, "can't delete autoforks with delStar");
		return FALSE;
	}
// find the block we are to remove
	Block* st = blockWithName (starname);
	if (st == NULL) {
		noInstance (starname, name());
		return 0;
	}
// this should always work, check anyway
	if (!removeBlock(*st)) {
		Error::abortRun (*this, "unexpected delStar failure!");
		return 0;
	}
// delete the object
	LOG_DEL; delete st;
// add action to list.  Yes, when we clone, a star will be created and
// then deleted.
	actionList += "s";
	actionList += starname;
	return TRUE;
}

// Add a porthole to the galaxy; alias it to a star porthole.
int
InterpGalaxy::alias(const char* galportname,const char* starname,
		    const char* portname) {
	galportname = hashstring (galportname);
// first get the portname for the contained star
	GenericPort *ph = findGenPort (starname, portname);
	if (ph == NULL) return FALSE;
// create new galaxy port, add to galaxy, do the alias
	DataType dType = ph->type();
	if (ph->isItMulti()) {
		GalMultiPort *p = new GalMultiPort(*(MultiPortHole*)ph);
		addPort(p->setPort(galportname,this,dType));
	}
	else {
		GalPort *p = new GalPort(*(PortHole*)ph);
		addPort(p->setPort(galportname,this,dType));
	}
// add action to list
	actionList += "A";
	actionList += galportname;
	actionList += starname;
	actionList += portname;
	return TRUE;
}

// Add a node (that is, a persistent Geodesic) for netlist-style
// connections.
int
InterpGalaxy::addNode (const char* nodename) {
	nodename = hashstring (nodename);
	// get a geodesic appropriate for the current domain, add to list
	// FIXME: Memory leak as many domains (e.g. CG56) return dynamic memory
	Geodesic& geo = Domain::named(domain())->newGeo(TRUE);
	geo.setNameParent (nodename, this);
	nodes.put(geo);
	// make actionList entry
	actionList += "n";
	actionList += nodename;
	return TRUE;
}

// Delete a node from the nodelist.
int
InterpGalaxy::delNode (const char* nodename) {
	Geodesic *g = nodes.nodeWithName (nodename);
	if (g == NULL) {
		noInstance (nodename, name());
		return FALSE;
	}
	if (!nodes.remove(g)) {
		Error::abortRun (*this, "delNode failed!?!");
		return FALSE;
	}
	// add to action list
	actionList += "x";
	actionList += nodename;
	return TRUE;
}

// Connect a porthole to a node.
int
InterpGalaxy::nodeConnect (const char* star, const char* port,
			   const char* node, const char* initDelayValues) {
	if ((initDelayValues == 0) || (*initDelayValues == 0))
	  initDelayValues = &emptyDelay;
	GenericPort *ph = findGenericPort (star, port);
	if (ph == NULL) return FALSE;
	Geodesic *g = nodes.nodeWithName (node);
	if (g == NULL) {
		noInstance (node, name());
		return FALSE;
	}

	// add original forms of parameters to action list
	actionList += "c";
	actionList += star;
	actionList += port;
	actionList += node;
	actionList += initDelayValues;

	if (ph->isItOutput()) {
		// Caution: this can change initDelayValues!
		int numDelays = getNumDelays(this, initDelayValues);
		if (!g->setSourcePort (*ph, numDelays, initDelayValues))
		  return FALSE;
	} else if (*initDelayValues) {
	  Error::abortRun(*ph,
			  "delay not allowed when nodeConnecting an input");
	  return FALSE;
	} else if (!g->setDestPort (*ph)) return FALSE;

	// If variable delay, add connection to initList to get reinitialized.
	if (*initDelayValues)
	  registerInit("c", ph, initDelayValues, NULL, node);

	return TRUE;
}

// disconnect a porthole from whatever it is connected to.
int
InterpGalaxy::disconnect (const char* star, const char* port) {
	PortHole* ph = findPortHole (star, port);
	if (ph == NULL) return FALSE;
	ph->disconnect();
// add to actionList.  Yes, when we clone, clone will connect and
// then disconnect.
	actionList += "d";
	actionList += star;
	actionList += port;
	return TRUE;
}

// add a state to the galaxy.
int
InterpGalaxy::addState (const char* statename, const char* stateclass, const char* statevalue) {
        statename = hashstring (statename);
	statevalue = hashstring (statevalue);
        State *src = KnownState::clone(stateclass);
        if (src == 0) return FALSE;
        Block::addState(src->setState(statename,this,statevalue));
//add action to list
        actionList += "T";
        actionList += statename;
        actionList += stateclass;
        actionList += statevalue;
        return TRUE;
}

// change a value of a state of a block within the galaxy.
// with blockname = "this", also works for states of the galaxy.
int
InterpGalaxy::setState (const char* blockname, const char* statename, const char* statevalue) {
        statevalue = hashstring (statevalue);
	if(!strcmp(blockname, "this")) {
		State *src = stateWithName(statename);
		if (src == 0) {
			noInstance (statename, name());
			return FALSE;
		}
		Block::setState(statename,statevalue);
	}
	else {
		Block* blk = blockWithName(blockname);
		if (blk ==  0) {
			noInstance (blockname, name());
			return FALSE;
		}
		State *src = blk->stateWithName(statename);
		if(src == 0) {
			noInstance (statename, blockname);
			return FALSE;
		}
		blk->setState(statename,statevalue);
	}
//add action to list
        actionList += "R";
	actionList += blockname;
        actionList += statename;
        actionList += statevalue;
        return TRUE;
}

// change the value of a state of the galaxy itself
int
InterpGalaxy :: setState(const char* stateName, const char* expression) {
	if (Block::setState(stateName, expression) == TRUE) {
	  // Add to action list
	  actionList += "E";
	  actionList += stateName;
	  actionList += expression;
	  return TRUE;
	} else {
	  return FALSE;
	}
}

// create a specified number of ports in a porthole.
int
InterpGalaxy :: numPorts (const char* star, const char* port, int num) {
	MultiPortHole *mp = findMPH (star, port);
	if (mp == NULL) return FALSE;

	// Quit if we already have the right number
	int np = mp->numberPorts ();
	if (np == num) return TRUE;

// make the ports (allow case where we've already connected some)
	for (int i = np; i < num; i++)
		mp->newPort();
// save on action list
	actionList += "N";
	actionList += star;
	actionList += port;
	actionList += num;
	return TRUE;
}

// modify the domain within the galaxy (for wormholes)
int
InterpGalaxy::setDomain (const char* name) {
	if (name == 0) {
		Error::error("InterpGalaxy::setDomain: null domain!");
		return FALSE;
	}
	// if we're already in the given domain, do nothing and return true
	if (strcmp (name, domain()) != 0) {
		if (numberBlocks() > 0) {
			Error::error (*this,
				      "Can't change domain, non-empty galaxy");
			return FALSE;
		}
		else if (!KnownBlock::validDomain(name)) {
			Error::error ("No such domain: ", name);
			return FALSE;
		}
		// OK, do it.
		name = hashstring(name);
		Galaxy::setDomain(name);
	}
	return TRUE;
}

// set the definition source
void
InterpGalaxy::setDefinitionSource(const char* source) {
	defSource = source ? hashstring(source) : ((const char *) NULL);
}

// clone function: uses copy constructor.
Block*
InterpGalaxy::clone() const { LOG_NEW; return new InterpGalaxy(*this);}
// makeNew function is the same.
Block*
InterpGalaxy::makeNew() const { return InterpGalaxy::clone();}

// DANGER WILL ROBINSON!!!  Casting actionList to const char* will cause all
// the action strings to be combined into one string.  This will break
// clone()!!!  Do not do it!
// However, "cout << actionList" is safe.

// This is the key to the works -- the function that makes an identical
// galaxy, given a galaxy.  It's the body of the copy constructor.

void
InterpGalaxy::copy(const InterpGalaxy& g) {
// make a new interpreted galaxy!  We do this by processing the action
// list.
	setDescriptor(g.descriptor());
	setDomain(g.domain());
	myClassName = g.myClassName;
	defSource = g.defSource;
	setNameParent(g.name(), NULL);

// process the action list
	processActionList(g.actionList);
}

void
InterpGalaxy::processActionList(const StringList& actionListToProcess) {
// process the action list
	int nacts = actionListToProcess.numPieces();
	StringListIter next(actionListToProcess);

	while (nacts > 0) {
		const char *a, *b, *c, *d, *action;
		const char *initDelayValues, *width;
		const char *tempString;

		action = next++;
		switch (action[0]) {

		case 'S':	// make a Star
			a = next++;
			b = next++;
			addStar (a, b);
			nacts -= 3;
			break;

		case 's':	// remove a Star
			a = next++;
			delStar (a);
			nacts -= 2;
			break;

		case 'C':	// add an internal connection
			a = next++;
			b = next++;
			c = next++;
			d = next++;
			tempString = next++;
			initDelayValues = tempString?hashstring(tempString):0;

			connect (a, b, c, d, initDelayValues);
			nacts -= 6;
			break;

		case 'B':	// add a bu connection
			a = next++;
			b = next++;
			c = next++;
			d = next++;
			width = next++;
			tempString = next++;
			initDelayValues = tempString?hashstring(tempString):0;
			busConnect (a, b, c, d, width, initDelayValues);
			nacts -= 7;
			break;

		case 'A':	// add an alias porthole to the galaxy
			a = next++;
			b = next++;
			c = next++;

			alias (a, b, c);
			nacts -= 4;
			break;

		case 'n':	// make a node (Geodesic)
			a = next++;
			addNode (a);
			nacts -= 2;
			break;

		case 'x':	// delete a node (Geodesic)
			a = next++;
			delNode (a);
			nacts -= 2;
			break;

		case 'c':	// connect a port to a node
			a = next++;
			b = next++;
			c = next++;
			tempString = next++;
			initDelayValues = tempString?hashstring(tempString):0;
			nodeConnect (a, b, c, initDelayValues);
			nacts -= 5;
			break;

		case 'd':	// disconnect a porthole
			a = next++;
			b = next++;
			disconnect (a, b);
			nacts -= 3;
			break;

               case 'T':       // add a state
                        a = next++;
                        b = next++;
                        c = next++;

                        addState (a, b, c);
                        nacts -= 4;
                        break;
               case 'R':       // set a state
                        a = next++;
                        b = next++;
                        c = next++;

                        setState (a, b, c);
                        nacts -= 4;
                        break;

		case 'N':	// make ports within multiports
			a = next++;
			b = next++;
			c = next++;

			numPorts (a, b, atoi (c));
			nacts -= 4;
			break;

		case 'E':	// set the state of a galaxy
			a = next++;
			b = next++;
			setState(a,b);
			nacts -= 3;
			break;
			
		default: // impossible (?)
			Error::abortRun (*this,
					 "Internal error in InterpGalaxy");
		}
	}

// Here we used to copy the initialization list.
// This is not necessary, however, because the initialization list
// should be correct based on the above connections created
// from processing the action list.
//	StringListIter nextI(g.initList);
//	initList.initialize();
//	const char* s;
//	while ((s = nextI++) != 0)
//		initList += s;
}

// add the galaxy to the known list (completing the definition of a galaxy
// class)
void
InterpGalaxy::addToKnownList(const char* definitionSource,
			     const char* outerDomain, Target* innerTarget) {
	const char* myName = hashstring(name());
	setNameParent(myName, parent());
// use my name as the "class name" for all objects that are cloned from me.
	myClassName = myName;
// save away the definition source info
	setDefinitionSource(definitionSource);

// If there was a domain change, this is a Wormhole.  Make the appropriate
// type of wormhole, add it to the list, and change back to outerDomain
// We always create a wormhole for certain domains (those for which
// isGalWorm is true)

// We also make a wormhole whenever an innerTarget is specified.

	Domain* od = Domain::named(outerDomain);
	if (!od) {
		Error::error("No such domain: ", outerDomain);
		return;
	}
	if (innerTarget || od->isGalWorm() ||
	    strcmp (outerDomain, domain()) != 0) {
		Star& s = od->newWorm(*this,innerTarget);
		setBlock (myName, &s);
		KnownBlock::addEntry (s, myName, 1, definitionSource);
	}
// If not, ordinary galaxy
	else {
		KnownBlock::addEntry (*this, myName, 1, definitionSource);
	}
}

// Function to allow us to use dotted names in interpreter show commands
// It works even if a star has a dot in its name.
Block* InterpGalaxy::blockWithDottedName (const char* dotname) {
	Block* b = blockWithName (dotname);
	if (b) return b;
	char buf[256];
	const char* p = strchr (dotname, '.');
	if (p == NULL) return 0;
	int n = p - dotname;
	strncpy (buf, dotname, n);
	buf[n] = 0;
	b = blockWithName (buf);
	if (!b || b->isItAtomic()) return NULL;
	return ((InterpGalaxy&)b->asGalaxy()).blockWithDottedName (p + 1);
}

void InterpGalaxy :: reset() {
	StringList actionListOriginal = actionList;
	zero();
	processActionList(actionListOriginal);
}	
	
// Destructor -- we have to wipe out all the manually added things.
// We don't need to do this for compiled Galaxys because the blocks,
// ports, and states are members.

// (this routine forms the body of the destructor, but has a different
// name so it can be used elsewhere)

void InterpGalaxy :: zero () {
	// remove other stuff
	DynamicGalaxy :: zero();
	// Clear action list
	actionList.initialize();
	// Clear init list
	initList.initialize();
}

// turns out that member and baseclass destructors do everything,
// so this is empty.
InterpGalaxy :: ~InterpGalaxy () {}

// preinitialize: redoes variable-parameter connections
//
// We formerly did this by overriding initialize(), but now we must do it
// in preinitialize() so that bus widths are correct before HOF star
// preinitialization runs.
// NOTE: with nested galaxies it is possible for this to be called more
// than once.  Thus we have to be sure that we will not undo anything
// done by a HOF star.  This implies a couple of subtle interactions:
// 1. A connection to a HOF star must not get remade.  We handle this
//    correctly by silently ignoring any connections involving no-longer-
//    extant stars.  Since Galaxy::preinitialize() destroys the HOF stars
//    before exiting, a subsequent new preinitialize will not find them.
// 2. Any delays etc. on connections made by HOF stars must be registered
//    in my initList.  HOF stars are responsible for calling registerInit()
//    to tell me about these.
void
InterpGalaxy::preinitialize () {
	int nacts = initList.numPieces();
	// Initialize galaxy's states in case bus width/delay exprs need them
	if (nacts > 0)
	  initState();
	StringListIter next(initList);
	int err = 0;
	// Note that while processing the initList, we ignore errors due to
	// missing stars or portholes.  Such errors are caused by HOF stars
	// that have been removed in the preinit phase of a previous run
	// (or perhaps even this run, if we are a subgalaxy).
	// The err counter is kept in case it is needed for debugging.
	while (nacts > 0) {
		const char *a, *b, *c, *d, *initDelayValues, *action;
		const char *tempString;
		int width;
		action = next++;
		switch (action[0]) {
		case 'B':
			a = next++;
			b = next++;
			c = next++;
			d = next++;
			width = evalExp(this,next++,"busWidth");
			tempString = next++;
			initDelayValues = tempString?hashstring(tempString):0;
			{
			  MultiPortHole* s = findMPHNE (a, b);
			  MultiPortHole* dp = findMPHNE (c, d);
			  if (s == 0 || dp == 0) err++;
			  else s->busConnect(*dp, width, 0, initDelayValues);
			}
			nacts -= 7;
			break;
		case 'C':
			a = next++;
			b = next++;
			c = next++;
			d = next++;
			tempString = next++;
			initDelayValues = tempString?hashstring(tempString):0;
			{
				PortHole* s = findPortHoleNE(a, b);
				if (s == 0) err++;
				else {
					s->setDelay(0, initDelayValues);
				}
			}
			nacts -= 6;
			break;
		case 'c':
			a = next++;
			b = next++;
			c = next++;
			tempString = next++;
			initDelayValues = tempString?hashstring(tempString):0;
			{
				Geodesic *g = nodes.nodeWithName(c);
				if (g == 0) err++;
				else g->setDelay(0, initDelayValues);
			}
			nacts -= 5;
			break;
		default:
			err++;
		}
	}
	Galaxy::preinitialize();
}

// NodeList methods.  These could be inline except that that would
// require earlier inclusion of Geodesic.
void NodeList::put(Geodesic& g) {
	NamedObjList::put(g);
}

int NodeList::remove(Geodesic* g) {
	return NamedObjList::remove(g);
}

// isa
ISA_FUNC(InterpGalaxy,Galaxy);
