/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 3/18/90
 Revisions:
	5/26/90 - I. Kuroda
		add InterpGalaxy::addState and InterpGalaxy::setState

 InterpGalaxy is a galaxy that may dynamically be added to, as well
 as cloned.

*******************************************************************/
#include "InterpGalaxy.h"
#include "StringList.h"
#include "Output.h"
#include "Connect.h"
#include "Geodesic.h"
#include "string.h"
#include "Domain.h"

extern Error errorHandler;

// Report an error: no such star or porthole

static void noInstance(const char* star,const char* gal) {
	StringList msg;
	msg = "No instance of \"";
	msg += star;
	msg += "\" in \"";
	msg += gal;
	msg += "\"\n";
	errorHandler.error (msg);
}

// Find a port or multiport -- preserve its "identity" -- things
// can call isItMulti on the result, say
GenericPort *
InterpGalaxy::findGenericPort (const char* star,const char* port) {
	Block *st = blockWithName(star);
	if (st == NULL) {
		noInstance (star, readName());
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
		noInstance (star, readName());
		return 0;
	}
	PortHole *ph = st->portWithName(port);
	if (ph == NULL) noInstance (port, star);
	return ph;
}

// Form a point-to-point connection.
int
InterpGalaxy::connect(const char* srcStar,const char* srcPipe,
		      const char* dstStar,const char* dstPipe,
		      int numberDelays=0) {
// Get the source and destination ports
	PortHole *srcP = findPortHole (srcStar, srcPipe);
	PortHole *dstP = findPortHole (dstStar, dstPipe);
	if (srcP == NULL || dstP == NULL) return FALSE;

	srcPipe = savestring(srcPipe);
	srcStar = savestring(srcStar);
	dstPipe = savestring(dstPipe);
	dstStar = savestring(dstStar);
// add the action to the list
	actionList += "C";
	actionList += srcStar;
	actionList += srcPipe;
	actionList += dstStar;
	actionList += dstPipe;
	actionList += numberDelays;
// Now it's obvious:
	connect (*srcP, *dstP, numberDelays);
	return TRUE;
}

// Add a star to the galaxy.
int
InterpGalaxy::addStar(const char* starname,const char* starclass) {
	starname = savestring (starname);
	starclass = savestring (starclass);
	Block *src = KnownBlock::clone(starclass);
	if (src == 0) return FALSE;
	addBlock(src->setBlock(starname,this));
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
	if (strncmp (starname, "!af", 3) == 0) {
		Error::abortRun (*this, "can't delete autoforks with delStar");
		return FALSE;
	}
// find the block we are to remove
	Block* st = blockWithName (starname);
	if (st == NULL) {
		noInstance (starname, readName());
		return 0;
	}
// this should always work, check anyway
	if (!removeBlock(*st)) {
		Error::abortRun (*this, "unexpected delStar failure!");
		return 0;
	}
// delete the object
	delete st;
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
	galportname = savestring (galportname);
// first get the portname for the contained star
	PortHole *ph = findPortHole (starname, portname);
	if (ph == NULL) return FALSE;
	portname = savestring (portname);
	starname = savestring (starname);
// create new galaxy port, add to galaxy, do the alias
	if (ph->isItInput()) {
		InPortHole *p = new InPortHole;
		addPort(p->setPort(galportname,this));
		alias (*p, *ph);
	}
	else {
		OutPortHole *p = new OutPortHole;
		addPort(p->setPort(galportname,this));
		alias (*p, *ph);
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
	nodename = savestring (nodename);
	// get a geodesic appropriate for the current domain, add to list
	Geodesic& geo = Domain::named(KnownBlock::domain())->newNode();
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
		noInstance (nodename, readName());
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
			   const char* node, int delay) {
	GenericPort *ph = findGenericPort (star, port);
	if (ph == NULL) return FALSE;
	Geodesic *g = nodes.nodeWithName (node);
	if (g == NULL) {
		noInstance (node, readName());
		return FALSE;
	}
	star = savestring (star);
	port = savestring (port);
	node = savestring (node);
	if (ph->isItOutput()) {
		if (!g->setSourcePort (*ph, delay)) return FALSE;
	}
	else if (delay) {
		Error::abortRun ("delay not allowed when nodeConnecting an input");
		return FALSE;
	}
	else if (!g->setDestPort (*ph)) return FALSE;

	// add to action list
	actionList += "c";
	actionList += star;
	actionList += port;
	actionList += node;
	actionList += delay;
	return TRUE;
}

// disconnect a porthole from whatever it is connected to.
int
InterpGalaxy::disconnect (const char* star, const char* port) {
	PortHole* ph = findPortHole (star, port);
	if (ph == NULL) return FALSE;
	star = savestring (star);
	port = savestring (port);
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
        statename = savestring (statename);
        stateclass = savestring (stateclass);
        statevalue = savestring (statevalue);
        State *src = KnownState::clone(stateclass);
        if (src == 0) return FALSE;
        addState(src->setState(statename,this,statevalue));
//add action to list
        actionList += "T";
        actionList += statename;
        actionList += stateclass;
        actionList += statevalue;
        return TRUE;
}

// change a value of a state within the galaxy.
int
InterpGalaxy::setState (const char* blockname, const char* statename, const char* statevalue) {
        blockname = savestring (blockname);
        statename = savestring (statename);
        statevalue = savestring (statevalue);
	if(!strcmp(blockname, "this")) {
		State *src = stateWithName(statename);
		if (src == 0) return FALSE;
		setState(statename,statevalue);
	}
	else {
		Block* blk = blockWithName(blockname);
		if (blk ==  0) {
			noInstance (blockname, readName());
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

// create a specified number of ports in a porthole.
int
InterpGalaxy :: numPorts (const char* star, const char* port, int num) {
	Block *st = blockWithName(star);
	if (st == NULL) {
		noInstance (star, readName());
		return FALSE;
	}
	MultiPortHole *mp = st->multiPortWithName(port);
	if (mp == NULL) {
		StringList msg;
		msg = "No MultiPortHole named \"";
		msg += port;
		msg += "\" in \"";
		msg += star;
		msg += "\"";
		errorHandler.error (msg);
		return FALSE;
	}

	// Quit if we already have the right number
	int np = mp->numberPorts ();
	if (np == num) return TRUE;

	star = savestring (star);
	port = savestring (port);
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
	actionList += "D";
	actionList += savestring (name);
	myDomain = savestring (name);
	// if we're already in the given domain, do nothing and return true
	if (strcmp (name, KnownBlock::domain()) == 0) return TRUE;
	if (numberBlocks() > 0) {
		errorHandler.error ("Can't change domain, non-empty galaxy");
		return FALSE;
	}
	if (!KnownBlock::setDomain (name)) return FALSE;
	return TRUE;
}

// DANGER WILL ROBINSON!!!  Casting actionList to char* will cause all
// the action strings to be combined into one string.  This will break
// clone()!!!  Do not do it!

// If you must print the action list do something like
// for (i = actionList.size(); i > 0; i--) cout << actionList.next() << "\n";

// This is the key to the works -- the function that makes an identical
// galaxy, given a galaxy.
Block* 
InterpGalaxy::clone() const {
// make a new interpreted galaxy!  We do this by processing the action
// list.
	InterpGalaxy* gal = new InterpGalaxy;
	gal->descriptor = descriptor;
	gal->setNameParent(readName(), NULL);
	const char* oldDom = NULL; // old domain

// process the action list
	int nacts = actionList.size();
	StringListIter next(actionList);

	while (nacts > 0) {
		const char *a, *b, *c, *d, *action;
		int ndelay;

		action = next++;
		switch (action[0]) {

		case 'S':	// make a Star
			a = next++;
			b = next++;
			gal->addStar (a, b);
			nacts -= 3;
			break;

		case 's':	// remove a Star
			a = next++;
			gal->delStar (a);
			nacts -= 2;
			break;

		case 'C':	// add an internal connection
			a = next++;
			b = next++;
			c = next++;
			d = next++;
			ndelay = atoi(next++);

			gal->connect (a, b, c, d, ndelay);
			nacts -= 6;
			break;

		case 'A':	// add an alias porthole to the galaxy
			a = next++;
			b = next++;
			c = next++;

			gal->alias (a, b, c);
			nacts -= 4;
			break;

		case 'n':	// make a node (Geodesic)
			a = next++;
			gal->addNode (a);
			nacts -= 2;
			break;

		case 'x':	// delete a node (Geodesic)
			a = next++;
			gal->delNode (a);
			nacts -= 2;
			break;

		case 'c':	// connect a port to a node
			a = next++;
			b = next++;
			c = next++;
			ndelay = atoi(next++);
			gal->nodeConnect (a, b, c, ndelay);
			nacts -= 5;
			break;

		case 'd':	// disconnect a porthole
			a = next++;
			b = next++;
			gal->disconnect (a, b);
			nacts -= 3;
			break;

               case 'T':       // add a state
                        a = next++;
                        b = next++;
                        c = next++;

                        gal->addState (a, b, c);
                        nacts -= 4;
                        break;
               case 'R':       // set a state
                        a = next++;
                        b = next++;
                        c = next++;

                        gal->setState (a, b, c);
                        nacts -= 4;
                        break;

		case 'N':	// make ports within multiports
			a = next++;
			b = next++;
			c = next++;

			gal->numPorts (a, b, atoi (c));
			nacts -= 4;
			break;

		case 'D':	// change the domain
			oldDom = KnownBlock::domain();
			a = next++;
			gal->setDomain (a);
			nacts -= 2;
			break;

		default:
			Error::abortRun ("Internal error in InterpGalaxy");
		}
	}

// if we're producing a wormhole, change the domain back
	if (oldDom) KnownBlock::setDomain (oldDom);
	return gal;
}

// add the galaxy to the known list (completing the definition of a galaxy
// class)
void
InterpGalaxy::addToKnownList(const char* outerDomain) {
	const char* myName = savestring(readName());
	setNameParent(myName, parent());

// If there was a domain change, this is a Wormhole.  Make the appropriate
// type of wormhole, add it to the list, and change back to outerDomain
// We always create a wormhole for certain domains (those for which
// isGalWorm is true)

	if (Domain::named(outerDomain)->isGalWorm() ||
	    strcmp (outerDomain, KnownBlock::domain()) != 0) {
		Star& s = Domain::named(outerDomain)->newWorm(*this);
		setBlock (myName, &s);
		KnownBlock::addEntry (s, myName, 1);
		KnownBlock::setDomain (outerDomain);
	}
// If not, ordinary galaxy
	else {
		KnownBlock::addEntry (*this, myName, 1);
	}
}

// Function to allow us to use dotted names in interpreter show commands
// It works even if a star has a dot in its name.
Block* InterpGalaxy::blockWithDottedName (const char* dotname) {
	Block* b = blockWithName (dotname);
	if (b) return b;
	char buf[256];
	const char* p = index (dotname, '.');
	if (p == NULL) return 0;
	int n = p - dotname;
	strncpy (buf, dotname, n);
	buf[n] = 0;
	b = blockWithName (buf);
	if (b->isItAtomic()) return NULL;
	return ((InterpGalaxy&)b->asGalaxy()).blockWithDottedName (p + 1);
}

// Destructor -- we have to wipe out all the manually added things.
// We don't need to do this for compiled Galaxys because the blocks,
// ports, and states are members.

InterpGalaxy :: ~InterpGalaxy () {
	// delete permanent nodes
	NodeListIter nextn(nodes);
	for (int i = nodes.size(); i > 0; i--)
		delete nextn++;

	// delete component blocks
	GalTopBlockIter nextb(*this);
	for (i = numberBlocks(); i > 0; i--)
		delete nextb++;

	// delete ports
	BlockPortIter nextp(*this);
	for (i = numberPorts(); i > 0; i--)
		delete nextp++;

	// delete states
	BlockStateIter nexts(*this);
	for (i = numberStates(); i > 0; i--)
		delete nexts++;
}

// function to find Node with given name, or NULL if no match
Geodesic*
NodeList::nodeWithName (const char* ident) {
	Geodesic *g;
	NodeListIter next(*this);
	while ((g = next++) != 0) {
		if (strcmp(ident, g->readName()) == 0)
			return g;
	}
	return NULL;
}
