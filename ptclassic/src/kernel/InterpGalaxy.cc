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

void
InterpGalaxy::connect(const char* srcStar,const char* srcPipe,
		      const char* dstStar,const char* dstPipe,
		      int numberDelays=0) {
// Get the source and destination ports
	PortHole *srcP = findPortHole (srcStar, srcPipe);
	PortHole *dstP = findPortHole (dstStar, dstPipe);
	if (srcP == NULL || dstP == NULL) return;

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
}

void
InterpGalaxy::addStar(const char* starname,const char* starclass) {
	starname = savestring (starname);
	starclass = savestring (starclass);
	Block *src = KnownBlock::clone(starclass);
	if (src == 0) return;
	addBlock(src->setBlock(starname,this));
//add action to list
	actionList += "S";
	actionList += starname;
	actionList += starclass;
	return;
}

void
InterpGalaxy::alias(const char* galportname,const char* starname,
		    const char* portname) {
	galportname = savestring (galportname);
// first get the portname for the contained star
	PortHole *ph = findPortHole (starname, portname);
	if (ph == NULL) return;
	portname = ph->readName();// safe copy
	starname = ph->parent()->readName();// safe copy
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
}

void
InterpGalaxy::addState (const char* statename, const char* stateclass, const char* statevalue) {
        statename = savestring (statename);
        stateclass = savestring (stateclass);
        statevalue = savestring (statevalue);
        State *src = KnownState::clone(stateclass);
        if (src == 0) return;
        addState(src->setState(statename,this,statevalue));
        initState();
//add action to list
        actionList += "T";
        actionList += statename;
        actionList += stateclass;
        actionList += statevalue;
        return;
}

void
InterpGalaxy::setState (const char* blockname, const char* statename, const char* statevalue) {
        blockname = savestring (blockname);
        statename = savestring (statename);
        statevalue = savestring (statevalue);
	if(!strcmp(blockname, "this")) {
		State *src = stateWithName(statename);
		if (src == 0) return;
		setState(statename,statevalue);
		initState();
	}
	else {
		Block* blk = blockWithName(blockname);
		if (blk ==  0) return;	
		State *src = blk->stateWithName(statename);
		if(src == 0) return;
		blk->setState(statename,statevalue);
		initState();
	}
//add action to list
        actionList += "R";
	actionList += blockname;
        actionList += statename;
        actionList += statevalue;
        return;
}

void
InterpGalaxy :: numPorts (const char* star, const char* port, int num) {
	Block *st = blockWithName(star);
	if (st == NULL) {
		noInstance (star, readName());
		return;
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
		return;
	}

	// Quit if we already have the right number
	int np = mp->numberPorts ();
	if (np == num) return;

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
	return;
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
			nacts -= 3;
			break;

		default:
			errorHandler.error ("Internal error in InterpGalaxy");
		}
	}

	return gal;
}

InterpGalaxy::addToKnownList() {
	KnownBlock *p = new KnownBlock(*this,savestring(readName()));
	delete p;
}
