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
#include "string.h"
#include "Domain.h"

extern Error errorHandler;


// Make a copy of a new string in a safe place.  This is needed because
// we may be fed strings from an interpreter that are later written over.
static char*
savestring (const char* txt) {
	char* s = new char[strlen(txt)+1];
	return strcpy (s, txt);
}

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
		actionList += "I";
		InPortHole *p = new InPortHole;
		addPort(p->setPort(galportname,this));
		alias (*p, *ph);
	}
	else {
		actionList += "O";
		OutPortHole *p = new OutPortHole;
		addPort(p->setPort(galportname,this));
		alias (*p, *ph);
	}
// add action to list
	actionList += galportname;
	actionList += starname;
	actionList += portname;
	return TRUE;
}

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

int
InterpGalaxy::setDomain (const char* name) {
	myDomain = savestring (name);
	// if we're already in the given domain, do nothing and return true
	if (strcmp (name, KnownBlock::domain()) == 0) return TRUE;
	if (numberBlocks() > 0) {
		errorHandler.error ("Can't change domain, non-empty galaxy");
		return FALSE;
	}
	if (!KnownBlock::setDomain (name)) return FALSE;
	actionList += "D";
	actionList += savestring (name);
	return TRUE;
}

// DANGER WILL ROBINSON!!!  Casting actionList to char* will cause all
// the action strings to be combined into one string.  This will break
// clone()!!!  Do not do it!

// If you must print the action list do something like
// for (i = actionList.size(); i > 0; i--) cout << actionList.next() << "\n";

Block* 
InterpGalaxy::clone() {
// make a new interpreted galaxy!  We do this by processing the action
// list.
	InterpGalaxy* gal = new InterpGalaxy;
	gal->descriptor = descriptor;
	gal->setNameParent(readName(), NULL);
	const char* oldDom = NULL; // old domain

// process the action list
	int nacts = actionList.size();
	actionList.reset();

	while (nacts > 0) {
		const char *a, *b, *c, *d, *action;
		int ndelay;

		action = actionList.next();
		switch (action[0]) {
		case 'S':	// make a Star
			a = actionList.next();
			b = actionList.next();
			gal->addStar (a, b);
			nacts -= 3;
			break;
		case 'C':	// add an internal connection
			a = actionList.next();
			b = actionList.next();
			c = actionList.next();
			d = actionList.next();
			ndelay = atoi(actionList.next());

			gal->connect (a, b, c, d, ndelay);
			nacts -= 6;
			break;
		case 'I':	// add an alias porthole
		case 'O':
			a = actionList.next();
			b = actionList.next();
			c = actionList.next();

			gal->alias (a, b, c);
			nacts -= 4;
			break;
               case 'T':       // add a state
                        a = actionList.next();
                        b = actionList.next();
                        c = actionList.next();

                        gal->addState (a, b, c);
                        nacts -= 4;
                        break;
               case 'R':       // set a state
                        a = actionList.next();
                        b = actionList.next();
                        c = actionList.next();

                        gal->setState (a, b, c);
                        nacts -= 4;
                        break;

		case 'N':	// make ports within multiports
			a = actionList.next();
			b = actionList.next();
			c = actionList.next();

			gal->numPorts (a, b, atoi (c));
			nacts -= 4;
			break;

		case 'D':	// change the domain
			oldDom = KnownBlock::domain();
			a = actionList.next();
			gal->setDomain (a);
			nacts -= 2;
			break;

		default:
			errorHandler.error ("Internal error in InterpGalaxy");
		}
	}

// if we're producing a wormhole, change the domain back
	if (oldDom) KnownBlock::setDomain (oldDom);
	return gal;
}

void
InterpGalaxy::addToKnownList(const char* outerDomain) {
	const char* myName = savestring(readName());
	setNameParent(myName, parent());

// If there was a domain change, this is a Wormhole.  Make the appropriate
// type of wormhole, add it to the list, and change back to outerDomain
	if (strcmp (outerDomain, KnownBlock::domain()) != 0) {
		Star& s = Domain::named(outerDomain)->newWorm(*this);
		setBlock (myName, &s);
		KnownBlock *p = new KnownBlock(s, myName);
		delete p;
		KnownBlock::setDomain (outerDomain);
	}
// If not, ordinary galaxy
	else {
		KnownBlock *p = new KnownBlock(*this,myName);
		delete p;
	}
}

// Function to allow us to use dotted names in interpreter show commands
Block* InterpGalaxy::blockWithDottedName (const char* dotname) {
	char buf[256];
	const char* p = index (dotname, '.');
	if (p == NULL) return blockWithName (dotname);
	int n = p - dotname;
	strncpy (buf, dotname, n);
	buf[n] = 0;
	Block* b = blockWithName (buf);
	if (b->isItAtomic()) return NULL;
	return ((InterpGalaxy&)b->asGalaxy()).blockWithDottedName (p + 1);
}

// Destructor -- we have to wipe out all the manually added things.
// We don't need to do this for compiled Galaxys because the blocks,
// ports, and states are members.

InterpGalaxy :: ~InterpGalaxy () {
	for (int i = numberBlocks(); i > 0; i--)
		delete &nextBlock();
	for (i = numberPorts(); i > 0; i--)
		delete &nextPort();
	for (i = numberStates(); i > 0; i--)
		delete &nextState();
}
