static const char file_id[] = "DDFClustSched.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include "type.h"
#include "DDFClustSched.h"
#include "SDFScheduler.h"
#include "pt_fstream.h"
#include "EventHorizon.h"
#include <std.h>
	
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-1994 The Regents of the University of California.
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

 Programmer:  Soonhoi Ha, J. Buck

The basic dynamic scheduler is the class DynDFScheduler.
This derived class has restructuring code to do restructuring as well.

**************************************************************************/
	
/*******************************************************************
		Main DDF scheduler routines
*******************************************************************/

// correct numTokens value for each EventHorizon.
void renewNumTokens(Block* b, int flag);

	////////////////////////////
	// setup
	////////////////////////////

void DDFClustSched :: setup () {
	if (logFile && *logFile) {
		logstrm = new pt_ofstream(logFile);
		if (!*logstrm) {
			LOG_DEL; delete logstrm;
			return;
		}
	}

	// if amISDF is already TRUE, do SDFScheduling
	if (amISDF) {
		if (!galaxy()) {
			Error::abortRun("DDFClustSched: no galaxy!");
			return;
		}
		clearHalt();
		realSched->setup();
		return;
	}
	DynDFScheduler::setup();
}

void DDFClustSched::initStructures() {

	if (restructured == FALSE) {
		// fancy stuff...
		// auto-creation of SDF wormholes, decide the most efficient
		// scheduler (Case, For, DoWhile, Recur)
		LOG_DEL; delete cgal;
		LOG_NEW; cgal = new SDFClusterGal(*galaxy(), logstrm);
		makeSDFClusters(*cgal);
		selectScheduler(*cgal);
		restructured = TRUE;
	}

	if (!amISDF)
		DynDFScheduler::initStructures();

	Block* gpar = galaxy()->parent();
	if (gpar && gpar->isItWormhole()) 
		renewNumTokens(gpar, FALSE);
}


	////////////////////////////
	// selectScheduler
	////////////////////////////

void DDFClustSched :: selectScheduler(SDFClusterGal& cgalaxy) {

	// check whether the galaxy is SDF or not
	if (amISDF) {
		LOG_NEW; realSched = new SDFClustSched;
		realSched->setGalaxy(*galaxy());
		realSched->setup();
		Block* gpar = galaxy()->parent();
		if (gpar) 
			renewNumTokens(gpar, TRUE);
		return;
	}

	// now it is DDF. schedule the SDF clusters
	cgalaxy.genSubScheds();

	// set the sample rates of cluster portholes
	SDFClusterGalIter nextClust(cgalaxy);
	SDFCluster* c;
	while ((c = nextClust++) != 0) {
		if (!c->asBag()) continue;
		// Now, it is an SDFCluster. set the sample rates.
		SDFClustPortIter nextPort(*c);
		SDFClustPort *p;
		while ((p = nextPort++) != 0) {
			int i = p->numIO() * p->inPtr()->parentClust()->reps();
			p->setNumXfer(i);
		}
	}

	setGalaxy(cgalaxy);
}

// constructor: set default options
// By default, no restructuring is done, and a DDF scheduler is always used.
// To enable restrucuturing, you must call setParams.
//
DDFClustSched::DDFClustSched (const char* log): logFile(log), logstrm(0),
	realSched(0), cgal(0) {
	amISDF = FALSE;
	restructured = FALSE;
}

DDFClustSched::~DDFClustSched() { 
	LOG_DEL; delete realSched; 
	LOG_DEL; delete logstrm;
	LOG_DEL; delete cgal; }


	////////////////////////////
	// renewNumTokens
	////////////////////////////
 
// after compile-time scheduling, change the parameter
// of the EventHorizons.
 
void renewNumTokens (Block* b, int flag) {
 
	BlockPortIter nextp(*b);
	PortHole* p;
 
	while ((p = nextp++) != 0) {
		EventHorizon* eveP = p->asEH();
		PortHole* ghostP = eveP->ghostAsPort();
		PortHole* bP = ghostP->far();
		int num = bP->numXfer();
		DataFlowStar* s = (DataFlowStar*) bP->parent();
		if (flag == TRUE) {
			num = num * s->repetitions.num();
		} else {
			if (num == 0) num = 1;
		}
		ghostP->asEH()->setParams(num);
	}
}


	////////////////////////////
	// makeSDFClusters
	////////////////////////////

// In the DDF domain, we group SDF stars as a cluster.
// This grouping will improve the runtime speed by utilizing the
// compile-time SDF scheduler.
// Also, with modified topology, check whether it is a special domain
// or not : Case, For, DoWhile, Recur.

int DDFClustSched :: makeSDFClusters (SDFClusterGal& galaxy) {

	if (logstrm) {
		*logstrm << " clustering SDF stars: begin...\n";
	}

	SDFClusterGalIter nextClust(galaxy);

	amISDF = FALSE;
	int flagDDF = 0;

	SDFCluster* c;

	// reset visit flag
	while ((c = nextClust++) != 0)
		c->setVisit(0);

	nextClust.reset();
	while ((c = nextClust++) != 0) {
		if (c->asBag() || c->visited()) continue;

		SDFAtomCluster* as = (SDFAtomCluster*) c;
		if (!as->isSDF()) {
			flagDDF = TRUE;
			continue; // if DDF star, skip.
		}

		// now, this star is a SDF star and not clustered yet.
		// make a cluster.
		SDFClusterBag* curBag = galaxy.createBag();
		c->setVisit(1);
		if (!expandCluster(c, &galaxy, curBag)) return FALSE;
		curBag->absorb(c, &galaxy);
		nextClust.reset();
	}

	if (!flagDDF) amISDF = TRUE;

	if (logstrm) {
		*logstrm << " clustering SDF stars: completed...\n";
	}
	return TRUE;
}

	////////////////////////////
	// expandCluster
	////////////////////////////

// Once create a SDF cluster, expand it as much as possible.

int DDFClustSched :: expandCluster (SDFCluster* c, SDFClusterGal* galaxy, SDFClusterBag* bag) {
	SDFClustPortIter nextp(*c);
	SDFClustPort* p;

	while((p = nextp++) != 0) {
		if (p->numTokens()) continue;	// if there is a delay, skip
		SDFCluster* s = p->far()->parentClust();
		if (s->visited()) continue;
		if (s->asBag()) continue;
		SDFAtomCluster* as = (SDFAtomCluster*) s;
		if (!as->real().isSDF()) continue;	// DDF star, skip
		s->setVisit(1);
		if (!expandCluster(s, galaxy, bag)) return FALSE;
		bag->absorb(s, galaxy);
	}
	return TRUE;
}

/*******************************************************************
		Main DDF scheduler routines
*******************************************************************/

int DDFClustSched :: isSource(Star& s) {
	BlockPortIter nextp(s);
	PortHole* p;
	while ((p = nextp++) != 0) {
		if (p->isItInput()) return FALSE;
	}
	return TRUE;
}

// The strategy of the runtime DDF scheduler is as follows
// E = enabled actors
// D = deferrable enabled actors
// S = source actors
 
// while (E-D != 0) fire (E-D)
// if (E=0)
//   deadlock
// else
//   if (E=S) require unbounded memory, so policy is arbitrary
//             (this seems easy to prove)
//   else fire(E-S)
// 
// One iteration is defined to fire all clustered actors at most
// once.

int DDFClustSched :: run() {
	if (realSched) {
		realSched->setStopTime(getStopTime());
		return realSched->run();
	}
	if (!galaxy()) {
		Error::abortRun("No galaxy to run");
		return FALSE;
	}
	 
	if (haltRequested()) {
		Error::abortRun(*galaxy(), 
			": Can't continue after run-time error");
		return FALSE;
	}

	SDFClusterGalIter nextClust(*cgal);
	SDFCluster* c;

	// reset visit flag
	while ((c = nextClust++) != 0) c->setVisit(0);

	int live = sourceBlocks.size();	// no deadlock

	while (numFiring < stopTime && !haltRequested()) {
	
		// (1) fire all enabled actor at most once
		int touched = FALSE;
		do {
			touched = FALSE;
			nextClust.reset();
			while ((c = nextClust++) != 0) {
				if (c->visited()) continue;
				if (typeOfActor(c) == 1) { // enabled
					if (!c->run()) return FALSE;
					c->setVisit(1);
					touched = TRUE;
				}
			}
		} while (touched == TRUE);

		// (2) fire ALL deferrable actors.
		int deadlock = TRUE;
		touched = FALSE;
		nextClust.reset();
		while ((c = nextClust++) != 0) {
			int t = typeOfActor(c);
			if (t) { // enabled or deferrable or source
				deadlock = FALSE;
				if (!c->visited()) { 
					if (!c->run()) return FALSE;
					c->setVisit(1);
					touched = TRUE;
				}
			}
		} 
		if (touched == TRUE) continue;
		numFiring++;	

		// (3) If there is no enabled actor
		//     (3-1) if no source and no deferrable star, deadlocked
		//     (3-2) else if no deferrable star, unlimited memory
		if (deadlock) {
			if (!live && numFiring < stopTime) { // deadlocked
			    Error :: abortRun("deadlock detected: check for  ",
			       "insufficient initial tokens on feedback arcs");
			    return FALSE;
			} 
		}

		// end of ONE iteration
		// clear visit flag, check buffer overflow
		nextClust.reset();
		while ((c = nextClust++) != 0) {
			SDFClustPortIter nextPort(*c);
			SDFClustPort *p = 0;
			while ((p = nextPort++) != 0) {
				SDFClustPort* inp = p;
				while (inp->inPtr()) inp = inp->inPtr();
				if (p->isItInput()) {
					int x = inp->real().numTokens();
					if (x > maxToken) {
						reportArcOverflow(p, maxToken);
						return FALSE;
					}
				}
			}
			c->setVisit(0);
		}
	}
	return TRUE;
}

int DDFClustSched :: typeOfActor(SDFCluster* s) {
	int flag = 2;	// runnable and deferrable
			// if runnable and non-deferrable, flag = 1
	int block;	// 1 if blocked at input
			// 0 if non-blocked at input

	if (s->asBag()) {  // if s is a bag, it is a SDF cluster
		block = blockAtInput(s);
		if (block > 0) flag = 0; 	// not enabled
		else if (!isOutputDeferrable(s)) flag = 1; // enabled
		return flag;
	}

	SDFAtomCluster* as = (SDFAtomCluster*) s;
	DataFlowStar& ds = as->real();
	DFPortHole* wp = ds.waitPort();	// waitPort
	if (wp) {
		// check waitNum
		int nwait = ds.waitTokens();
		if (wp->numTokens() < nwait) {
			return 0;	// not enabled.
		}
		if (!isOutputDeferrable(s)) flag = 1;
		return flag;
	} 
	block = blockAtInput(s);
	if (block > 0) flag = 0; 	// not enabled
	else if (!isOutputDeferrable(s)) flag = 1; // enabled
	return flag;
}

// return 1 if not enabled
// return 0 if enabled
int DDFClustSched :: blockAtInput(SDFCluster* s) {
	SDFClustPortIter nextPort(*s);
	SDFClustPort *p = 0;
	int src = 0;		// block is a source
	while ((p = nextPort++) != 0) {
		SDFClustPort* inp = p;
		while (inp->inPtr()) inp = inp->inPtr();
		if (p->isItInput()) {
			int x = inp->real().numTokens();
			if (p->numTokens() > maxToken) overFlowArc = p;
			if (x < p->numIO()) {
				return 1;
			}
		}
	}
	return src;
}

int DDFClustSched :: isOutputDeferrable(SDFCluster* s) {
	SDFClustPortIter nextPort(*s);
	SDFClustPort *p;
	while ((p = nextPort++) != 0) {
		if (p->isItInput()) continue;
		SDFClustPort* inp = p;
		while (inp->inPtr()) inp = inp->inPtr();
		DFPortHole& dp = inp->real();
		DataFlowStar* ds = (DataFlowStar*) dp.far()->parent();
		DFPortHole* wp = ds->waitPort();
		if (wp && (dp.far() == (PortHole*) wp)) {
			// check waitNum
			int nwait = ds->waitTokens();
			if (wp->numTokens() < nwait) {
				return 0;	// not deferrable.
			}
		} else if (wp) {
			continue;
		} else {
			if (p->far()->numIO() > dp.numTokens()) 
				return 0; // not deferrable
		}
	}
	return 1;
}
		
