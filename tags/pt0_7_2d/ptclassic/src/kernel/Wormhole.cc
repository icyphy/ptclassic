static const char file_id[] = "Wormhole.cc";
/******************************************************************
Version identification:
@(#)Wormhole.cc	2.44	05/15/98

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

Wormhole::Wormhole(Star& s,Galaxy& g,const char* targetName) : 
    Runnable(targetName,g.domain(),&g), gal(g), selfStar(s) 
{
	// set up the parent pointer of inner Galaxy
	g.setParent(&s);

	dynamicHorizons = FALSE;
}

Wormhole::Wormhole(Star& s,Galaxy& g,Target* innerTarget) : 
         Runnable(innerTarget,g.domain(),&g), gal(g), selfStar(s)
{
	// set up the parent pointer of inner Galaxy
	g.setParent(&s);

	dynamicHorizons = FALSE;
}

// function to form the name for the inner event horizon
// hashstring is efficient here because repeated porthole names like
// "input" and "output" are so common

const char* ghostName(const GenericPort& galp) {
	StringList buf = galp.name();
	buf << "(ghost)";
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

		// We may be converting a Galaxy whose contents are
		// already connected to the outside.  If so, we need to
		// connect the wormhole to the outside.  An example of
		// this is with Clusters.
		PortHole* far =  realGalp.far();
		int numDelays = realGalp.numInitDelays();
		const char* delayValues = realGalp.initDelayValues();
		if (far) realGalp.disconnect();
		
// separate rules for connecting inputs and outputs.
		if (galp.isItInput()) {
			EventHorizon& to = outSideDomain->newTo();
			PortHole& toPort = *to.asPort();
			EventHorizon& from = inSideDomain->newFrom();
			PortHole& fromPort = *from.asPort();
			// If event horizons of either the inside or
			// the outside domain transfer only one token
			// at a time, set numToken to be 1.
			if (to.onlyOne() || from.onlyOne()) numToken = 1;

			to.setEventHorizon(in, this, numToken);
			toPort.setPort(galp.name(),&selfStar,type);
			selfStar.addPort(toPort);
			
			from.setEventHorizon(in, this, numToken);
			fromPort.setPort(ghostName(galp),&selfStar,type);
			
			to.ghostConnect (from);
			fromPort.connect(realGalp,0);
			if (far) far->connect(toPort,numDelays,delayValues);
		}
		else {
			EventHorizon& to = inSideDomain->newTo();
			PortHole& toPort = *to.asPort();
			EventHorizon& from = outSideDomain->newFrom();
			PortHole& fromPort = *from.asPort();
			// If event horizons of either the inside or
			// the outside domain transfer only one token
			// at a time, set numToken to be 1.
			if (to.onlyOne() || from.onlyOne()) numToken = 1;

			from.setEventHorizon(out, this, numToken);
			fromPort.setPort(galp.name(),&selfStar,type);
			selfStar.addPort(fromPort);
			
			to.setEventHorizon(out, this, numToken);
			toPort.setPort(ghostName(galp),&selfStar,type);
			
			to.ghostConnect (from);
			realGalp.connect(toPort,0);
			if (far) fromPort.connect(*far,numDelays,delayValues);
		}
	}
// Take care of galaxy multi porthole
	BlockMPHIter mpi(gal);
	MultiPortHole* mp;
	while ((mp = mpi++) != 0) {
		GalMultiPort* gp = (GalMultiPort*) mp;
		LOG_NEW; WormMultiPort* wp = new WormMultiPort(this, *gp);
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
	myTarget()->resetStopTime(getStopTime());
}

// return the scheduler of the outer domain.
Scheduler* Wormhole :: outerSched() { return selfStar.parent()->scheduler(); }

// The following three methods of the Wormhole class define the default,
// common behavior of the "setup", "begin", and "go" methods of the
// XXXWormhole classes.  Those methods of the XXXWormhole classes should
// be defined to invoke these methods, augmented with domain-specific
// code if necessary.

// Default, common behavior of XXXWormhole::setup()
void Wormhole :: setup() {
	initTarget();
}

// Default, common behavior of XXXWormhole::begin()
void Wormhole :: begin() {
	if (myTarget())
		myTarget()->begin();
}

// Default, common behavior of XXXWormhole::go()
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
	BlockPortIter ports(selfStar);
	PortHole* p;

	while((p = ports++) != 0) {
		int numDelays = p->numInitDelays();
		const char* delayValues = p->initDelayValues();
		PortHole* tempP = p->far();
		EventHorizon* eveP = p->asEH();
		PortHole* inP = eveP->ghostAsPort()->far();
		if (tempP) tempP->disconnect(1);
		if (inP) inP->disconnect(1);

		// relink any aliases pointing at me to point to inner galaxy.
		if (inP) p->moveMyAliasesTo(*inP);

		// remove parent pointers, so portholes won't try to
		// remove themselves from their parent.
		eveP->ghostAsPort()->setPort("",0);
		eveP->asPort()->setPort("",0);
		// now delete the event horizon pair.
		LOG_DEL; delete eveP;
		ports.remove();
		
		// make new connection
		if (tempP && tempP->isItInput())
			inP->connect(*tempP, numDelays, delayValues);
		else if (tempP)
			tempP->connect(*inP, numDelays, delayValues);
	}

	// Set the inner galaxy name to the wormhole selfStar name.
	// The inner galaxy name may not be unique!
	gal.setName(selfStar.name());
	
	galP = 0;
	return &gal;
}
		

