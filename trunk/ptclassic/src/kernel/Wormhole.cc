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
	messageProcessingTime = 10.0; // default delay
	currentTime = 0.0;	      // default
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
		dataType type = galp.realPort().myType();
		if (galp.isItInput()) {
			EventHorizon& to = outSideDomain->newTo();
			EventHorizon& from = inSideDomain->newFrom();
			to.setPort(in, galp.readName(), this, &selfStar,
				   type);
			selfStar.addPort(to);
			from.setPort(in, galp.readName(), this, &selfStar,
				     type);
			to.ghostConnect (from);
			if (type == ANYTYPE) {
				to.inheritTypeFrom (from);
				from.inheritTypeFrom (galp);
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
				     ANYTYPE);
			selfStar.addPort(from);
			to.setPort(out, galp.readName(), this, &selfStar,
				   ANYTYPE);
			to.ghostConnect (from);
			to.inheritTypeFrom (from);
			galp.inheritTypeFrom (to);
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

/* void Wormhole :: checkSDF()
{
// check if the repetition member of the SDFstars at the wormhole
// boundary is one. If not, CURRENTLY, report as ERROR!
	if (!strcmp(type, SDFstring)) {
	   for (int i = numberPorts(); i > 0; i--) {
		PortHole* p = ((EventHorizon&) nextPort()).insidePort;
		SDFStar* s = (SDFStar*) p->parent();
		if (s->repetitions > 1)
			errorHandler.error("Repetition should be one:",
				s->readFullName());
	   }
	}
}
*/

