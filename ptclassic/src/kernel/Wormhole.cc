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
#include "Wormhole.h"
#include "StringList.h"
#include "Output.h"
#include "Domain.h"

extern Error errorHandler;

/*******************************************************************

	class Wormhole methods

********************************************************************/

// Constructor
// Here, Galaxy is the inner galaxy, Star points to the Star part
// of the wormhole.  That is, we would say
// SDFWormhole::SDFWormhole(Galaxy&g) : Wormhole(g,this)
// { buildEventHorizons();}

Wormhole::Wormhole(Star& s,Galaxy& g) : selfStar(s),
	Runnable(&Domain::domainOf(g)->newSched(),
		 Domain::domainOf(g)->domainName(),&g)
{
	// set up the parent pointer of inner Galaxy
	g.setNameParent("innerGal", &s);

	dynamicHorizons = FALSE;
}

// function to build the event horizons connecting the inner galaxy
// to the outside.

void Wormhole :: buildEventHorizons () {
	// check so this isn't done twice.
	if (selfStar.numberPorts() > 0) return;
	Domain* inSideDomain = Domain::domainOf(gal);
	Domain* outSideDomain = Domain::domainOf(selfStar);
// Take each of the galaxy ports and make a pair of EventHorizons; connect
// them together.
	for (int n = gal.numberPorts(); n>0; n--) {
		PortHole& galp = gal.nextPort();
		PortHole& realGalp = (PortHole&) galp.realPort();
		dataType type = realGalp.myType();
		int numToken = realGalp.numberTokens;
		if (galp.isItInput()) {
			EventHorizon& to = outSideDomain->newTo();
			EventHorizon& from = inSideDomain->newFrom();
			to.setPort(in, galp.readName(), this, &selfStar,
				   type, numToken);
			selfStar.addPort(to);
			from.setPort(in, galp.readName(), this, &selfStar,
				     type, numToken);
			to.ghostConnect (from);
			if (type == ANYTYPE) {
				realGalp.inheritTypeFrom (from);
				from.inheritTypeFrom (to);
			}
			from.connect(galp,0);
		}
		else {
// for outputs, we must propogate the type information back from
// whatever the output is connected to, so there are no type conversions
// across ghost connections.
			EventHorizon& to = inSideDomain->newTo();
			EventHorizon& from = outSideDomain->newFrom();
			from.setPort(out, galp.readName(), this, &selfStar,
				     type, numToken);
			selfStar.addPort(from);
			to.setPort(out, galp.readName(), this, &selfStar,
				   type, numToken);
			to.ghostConnect (from);
			if (type == ANYTYPE) {
				from.inheritTypeFrom (realGalp);
				to.inheritTypeFrom (from);
			} else {
				to.inheritTypeFrom (from);
				realGalp.inheritTypeFrom (to);
			}
			galp.connect(to,0);
		}
	}
	dynamicHorizons = TRUE;
	return;
}

Wormhole::~Wormhole () {
// do later
}
	
// method for printing info on a wormhole
StringList Wormhole :: print (int recursive) {
	StringList out;
// title, eg SDF in DE Wormhole: myNameHere
	out += Domain::domainOf(gal)->domainName();
	out += " in ";
	out += selfStar.domain();
	out += " Wormhole: ";
	out += selfStar.readFullName();
	out += "\nDescriptor: ";
	out += selfStar.readDescriptor();
	out += "\n";
	out += selfStar.printPorts ("wormhole");
	out += selfStar.printStates ("wormhole");
	out += "Blocks in the inner galaxy: ";
	if (recursive) {
		out += "------------------------\n";
		for (int i = gal.numberBlocks(); i > 0; i--)
			out += gal.nextBlock().printRecursive();
	}
	else {
		for (int i = gal.numberBlocks(); i>0; i--) {
			out += gal.nextBlock().readName();
			if (i > 1) out += ", ";
		}
		out += "\n";
	}
	return out;
}

// check input EventHorizons.
int Wormhole :: checkReady() {
	int flag = TRUE;
	for (int i = selfStar.numberPorts(); i > 0; i--) {
		EventHorizon& p = (EventHorizon&) selfStar.nextPort();
		if (p.isItInput()) {
			FromEventHorizon* fp = (FromEventHorizon*) p.ghostPort;
			if (!(fp->ready())) flag = FALSE;
		}
	}
	return flag;
}
