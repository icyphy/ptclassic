static const char file_id[] = "Wormhole.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

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
	g.setNameParent(g.readName(), &s);

	dynamicHorizons = FALSE;
}

Wormhole::Wormhole(Star& s,Galaxy& g,Target* innerTarget) : selfStar(s),
	Runnable(innerTarget,g.domain(),&g), gal(g)
{
	// set up the parent pointer of inner Galaxy
	g.setNameParent(g.readName(), &s);

	dynamicHorizons = FALSE;
}

// function to form the name for the inner event horizon
// hashstring is efficient here because repeated porthole names like
// "input" and "output" are so common

const char* ghostName(const GenericPort& galp) {
	char buf[80];
	strcpy (buf, galp.readName());
	strcat (buf, "(ghost)");
	return hashstring(buf);
}

// function to build the event horizons connecting the inner galaxy
// to the outside.

void Wormhole :: buildEventHorizons () {
	// check so this isn't done twice.
	if (selfStar.numberPorts() > 0 || !galP) return;
	Domain* inSideDomain = Domain::domainOf(gal);
	Domain* outSideDomain = Domain::domainOf(selfStar);
// Take each of the galaxy ports and make a pair of EventHorizons; connect
// them together.
	BlockPortIter next(gal);
	for (int n = gal.numberPorts(); n>0; n--) {
		PortHole& galp = *next++;
		PortHole& realGalp = (PortHole&) galp.realPort();
		DataType type = realGalp.myType();
		int numToken = realGalp.numXfer();
// separate rules for connecting inputs and outputs.
		if (galp.isItInput()) {
			EventHorizon& to = outSideDomain->newTo();
			EventHorizon& from = inSideDomain->newFrom();
			to.setPort(in, galp.readName(), this, &selfStar,
				   type, numToken);
			selfStar.addPort(to);
			from.setPort(in, ghostName(galp), this, &selfStar,
				     type, numToken);
			to.ghostConnect (from);
			from.inheritTypeFrom (realGalp);
			to.inheritTypeFrom (from);
			from.connect(realGalp,0);
		}
		else {
			EventHorizon& to = inSideDomain->newTo();
			EventHorizon& from = outSideDomain->newFrom();
			from.setPort(out, galp.readName(), this, &selfStar,
				     type, numToken);
			selfStar.addPort(from);
			to.setPort(out, ghostName(galp), this, &selfStar,
				   type, numToken);
			to.ghostConnect (from);
			to.inheritTypeFrom (realGalp);
			from.inheritTypeFrom (to);
			realGalp.connect(to,0);
		}
	}
// Take care of galaxy multi porthole
	BlockMPHIter mpi(gal);
	MultiPortHole* mp;
	while ((mp = mpi++) != 0) {
		LOG_NEW; WormMultiPort* wp = new WormMultiPort(this, *mp);
		wp->setPort(mp->readName(), &selfStar, mp->myType());
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
	EventHorizon* p;
	while ((p = (EventHorizon*)nextp++) != 0) {
		// remove parents
		p->ghostPort->PortHole::setPort("",0);
		p->PortHole::setPort("",0);
		LOG_DEL; delete p->ghostPort;
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
	out += selfStar.readFullName();
	out += "\nDescriptor: ";
	out += selfStar.readDescriptor();
	out += "\n";
	out += selfStar.printPorts ("wormhole");
// we use the states from the inner galaxy.
	out += gal.printStates ("wormhole");
	out += "Blocks in the inner galaxy: ";
	GalTopBlockIter next(gal);
	if (recursive) {
		out += "------------------------\n";
		for (int i = gal.numberBlocks(); i > 0; i--)
			out += (next++)->printRecursive();
	}
	else {
		for (int i = gal.numberBlocks(); i>0; i--) {
			out += (next++)->readName();
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
		EventHorizon& p = *(EventHorizon*) next++;
		if (p.isItInput()) {
			FromEventHorizon* fp = (FromEventHorizon*) p.ghostPort;
			if (!(fp->ready())) { flag = FALSE;
					      return flag ;}
		}
	}

	return flag;
}

// set scheduler stop time.  By default, argument is ignored.
void Wormhole :: setStopTime(float) {
	target->resetStopTime(getStopTime());
}

// arrange things after run
void Wormhole :: sumUp() {}

// should be redefined in the derived classes.  But, we can not
// use abstract virtual method by some strange reason (compiler bug!) 
// for now.
float Wormhole :: getStopTime() { return 0 ;}

