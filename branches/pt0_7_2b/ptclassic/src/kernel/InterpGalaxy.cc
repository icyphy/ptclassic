/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 3/18/90

 InterpGalaxy is a galaxy that may dynamically be added to, as well
 as cloned.

*******************************************************************/
#include "InterpGalaxy.h"
#include "StringList.h"
#include "Output.h"
#include "string.h"

extern Error errorHandler;


// Make a copy of a new string in a safe place.  This is needed because
// we may be fed strings from an interpreter that are later written over.
static char *
savestring (char *txt) {
	char *s = new char[strlen(txt)+1];
	return strcpy (s, txt);
}

// Report an error: no such star or porthole

static void noInstance(char *star,char *gal) {
	StringList msg;
	msg = "No instance of \"";
	msg += star;
	msg += "\" in \"";
	msg += gal;
	msg += "\"\n";
	errorHandler.error (msg);
}

PortHole *
InterpGalaxy::findPortHole (char *star,char *port) {
	Block *st = blockWithName(star);
	if (st == NULL) {
		noInstance (star, readName());
		return 0;
	}
	PortHole *ph = st->portWithName(port);
	if (ph == NULL) noInstance (port, star);
	return ph;
}

Geodesic &
InterpGalaxy::connect(char *srcStar,char *srcPipe,char *dstStar,char *dstPipe,
	int numberDelays=0) {
// Get the source and destination ports
	PortHole *srcP = findPortHole (srcStar, srcPipe);
	PortHole *dstP = findPortHole (dstStar, dstPipe);
	if (srcP == NULL || dstP == NULL) return;

	srcPipe = srcP->readName();
	srcStar = srcP->blockIamIn->readName();
	dstPipe = dstP->readName();
	dstStar = dstP->blockIamIn->readName();
// add the action to the list
	actionList += "C";
	actionList += srcStar;
	actionList += srcPipe;
	actionList += dstStar;
	actionList += dstPipe;
	actionList += numberDelays;
// Now it's obvious:
	return connect (*srcP, *dstP, numberDelays);
}

InterpGalaxy::addStar(char *starname,char *starclass) {
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

InterpGalaxy::alias(char *galportname,char *starname,char *portname) {
	galportname = savestring (galportname);
// first get the portname for the contained star
	PortHole *ph = findPortHole (starname, portname);
	if (ph == NULL) return;
	portname = ph->readName();// safe copy
	starname = ph->blockIamIn->readName();// safe copy
// create new galaxy port, add to galaxy, do the alias
	if (ph->isItInput()) {
		actionList += "I";
		InSDFPort *p = new InSDFPort;
		addPort(p->setPort(galportname,this));
		alias (*p, *ph);
	}
	else {
		actionList += "O";
		OutSDFPort *p = new OutSDFPort;
		addPort(p->setPort(galportname,this));
		alias (*p, *ph);
	}
// add action to list
	actionList += galportname;
	actionList += starname;
	actionList += portname;
}

Block *
InterpGalaxy::clone() {
// make a new interpreted galaxy!  We do this by processing the action
// list.
	InterpGalaxy* gal = new InterpGalaxy;
	gal->descriptor = descriptor;

// process the action list
	int nacts = actionList.size();
	actionList.reset();

	while (nacts > 0) {
		char *a, *b, *c, *d, *action;
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
