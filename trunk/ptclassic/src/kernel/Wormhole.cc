static const char file_id[] = "Wormhole.cc";
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

 Programmer: Soonhoi Ha 
 Date of creation: 3/19/90
 Date of modification: 5/24/90
 Date of modification: 5/31/90
 Date of modification: 6/13/90

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "Wormhole.h"
#include "Scheduler.h"
#include "StringList.h"
#include "Error.h"
#include "Domain.h"
#include <std.h>

extern Error errorHandler;

/*******************************************************************

	class Wormhole methods

********************************************************************/

// Constructor
// Here, Galaxy is the inner galaxy, Star points to the Star part
// of the wormhole.  That is, we would say
// SDFWormhole::SDFWormhole(Galaxy&g) : Wormhole(g,this)
// { buildEventHorizons();}

Wormhole::Wormhole(Star& s,Galaxy& g,const char* targetName) : selfStar(s),
	Runnable(targetName,g.domain(),&g), gal(g)
{
	// set up the parent pointer of inner Galaxy
	g.setNameParent(g.name(), &s);

	dynamicHorizons = FALSE;
}

Wormhole::Wormhole(Star& s,Galaxy& g,Target* innerTarget) : selfStar(s),
	Runnable(innerTarget,g.domain(),&g), gal(g)
{
	// set up the parent pointer of inner Galaxy
	g.setNameParent(g.name(), &s);

	dynamicHorizons = FALSE;
}

// function to form the name for the inner event horizon
// hashstring is efficient here because repeated porthole names like
// "input" and "output" are so common

const char* ghostName(const GenericPort& galp) {
	char buf[80];
	strcpy (buf, galp.name());
	strcat (buf, "(ghost)");
	return hashstring(buf);
}

// function to build the event horizons connecting the inner galaxy
// to the outside.

void Wormhole :: buildEventHorizons () {
	// check so this isn't done twice.
	if (selfStar.numberPorts() > 0 || !galP) return;
	Domain* inSideDomain = Domain::of(gal);
	Domain* outSideDomain = Domain::of(selfStar);
// Take each of the galaxy ports and make a pair of EventHorizons; connect
// them together.
	BlockPortIter next(gal);
	for (int n = gal.numberPorts(); n>0; n--) {
		PortHole& galp = *next++;
		PortHole& realGalp = (PortHole&) galp.realPort();
		DataType type = realGalp.type();
		int numToken = realGalp.numXfer();
// separate rules for connecting inputs and outputs.
		if (galp.isItInput()) {
			EventHorizon& to = outSideDomain->newTo();
			EventHorizon& from = inSideDomain->newFrom();
			to.setEventHorizon(in, galp.name(), this, 
				&selfStar, type, numToken);
			selfStar.addPort(*(to.asPort()));
			from.setEventHorizon(in, ghostName(galp), this, 
				&selfStar, type, numToken);
			to.ghostConnect (from);
			from.asPort()->inheritTypeFrom (realGalp);
			to.asPort()->inheritTypeFrom (*(from.asPort()));
			from.asPort()->connect(realGalp,0);
		}
		else {
			EventHorizon& to = inSideDomain->newTo();
			EventHorizon& from = outSideDomain->newFrom();
			from.setEventHorizon(out, galp.name(), this, 
				&selfStar, type, numToken);
			selfStar.addPort(*(from.asPort()));
			to.setEventHorizon(out, ghostName(galp), this, 
				&selfStar, type, numToken);
			to.ghostConnect (from);
			to.asPort()->inheritTypeFrom (realGalp);
			from.asPort()->inheritTypeFrom (*(to.asPort()));
			realGalp.connect(*(to.asPort()),0);
		}
	}
// Take care of galaxy multi porthole
	BlockMPHIter mpi(gal);
	MultiPortHole* mp;
	while ((mp = mpi++) != 0) {
		LOG_NEW; WormMultiPort* wp = new WormMultiPort(this, *mp);
		wp->setPort(mp->name(), &selfStar, mp->type());
		selfStar.addPort(*wp);
	}

	dynamicHorizons = TRUE;
	return;
}

// free the contents of the wormhole.  This isn't the destructor because
// we need to get the ordering right; instead, this function is called
// from the XXXWormhole destructor for each XXXWormhole.
void Wormhole::freeContents () {
	if (!dynamicHorizons || !galP) return;
	BlockPortIter nextp(selfStar);
	PortHole* p;
	while ((p = nextp++) != 0) {
		// remove parents
		p->asEH()->ghostAsPort()->setPort("",0);
		p->setPort("",0);
		LOG_DEL; delete p;
	}
	// delete the inner galaxy.
	LOG_DEL; delete galP; galP = 0;
}

// method for printing info on a wormhole
StringList Wormhole :: print (int recursive) const {
	StringList out;
// title, eg SDF in DE Wormhole: myNameHere
	out += gal.domain();
	out += " in ";
	out += selfStar.domain();
	out += " Wormhole: ";
	out += selfStar.fullName();
	out += "\nDescriptor: ";
	out += selfStar.descriptor();
	out += "\n";
	out += selfStar.printPorts ("wormhole",recursive);
// we use the states from the inner galaxy.
	out += gal.printStates ("wormhole",recursive);
	out += "Blocks in the inner galaxy: ";
	GalTopBlockIter next(gal);
	if (recursive) {
		out += "------------------------\n";
		for (int i = gal.numberBlocks(); i > 0; i--)
			out += (next++)->print(recursive);
	}
	else {
		for (int i = gal.numberBlocks(); i>0; i--) {
			out += (next++)->name();
			if (i > 1) out += ", ";
		}
		out += "\n";
	}
	return out;
}

// check input EventHorizons.
int Wormhole :: checkReady() const {
	int flag = TRUE;

	// check each porthole whether it is ready.
	BlockPortIter next(selfStar);
	for (int i = selfStar.numberPorts(); i > 0; i--) {
		PortHole* p = next++;
		if (p->isItInput()) {
			FromEventHorizon* fp = 
				(FromEventHorizon*) p->asEH()->ghostPort;
			if (!(fp->ready())) { flag = FALSE;
					      return flag ;}
		}
	}

	return flag;
}

// set scheduler stop time.  By default, argument is ignored.
void Wormhole :: setStopTime(double) {
	target->resetStopTime(getStopTime());
}

// return the scheduler of the outer domain.
Scheduler* Wormhole :: outerSched() { return selfStar.parent()->scheduler(); }

void Wormhole :: setup() {
	initTarget();
}

int Wormhole :: run() {
	if (!checkReady()) return TRUE;
	setStopTime(1.0);		// 1.0 is dummy value.
	if (!Runnable :: run()) return FALSE;
	sumUp();
	return TRUE;
}

// arrange things after run
void Wormhole :: sumUp() {}

// explode myself to expose the inside Galaxy.
Galaxy* Wormhole :: explode() {
	BlockPortIter nextp(selfStar);
	PortHole* p;

	while((p = nextp++) != 0) {
		int delay = p->numInitDelays();
		PortHole* tempP = p->far();
		EventHorizon* eveP = p->asEH();
		PortHole* inP = eveP->ghostAsPort()->far();
		GenericPort* alp = p->aliasFrom();
		tempP->disconnect(1);
		inP->disconnect(1);

		// remove parent pointers, so portholes won't try to
		// remove themselves from their parent.
		eveP->ghostAsPort()->setPort("",0);
		eveP->asPort()->setPort("",0);
		// now remove the event horizon pair.
		LOG_DEL; delete eveP;

		// make new connection
		if (tempP->isItInput())
			inP->connect(*tempP, delay);
		else
			tempP->connect(*inP, delay);

		// update aliase pointer if necessary.
		// since deleting EventHorizons may corrupt aliase pointers.
		if (alp != 0)
			alp->setAlias(*(inP->aliasFrom()));
	}
	galP = 0;
	return &gal;
}
		
