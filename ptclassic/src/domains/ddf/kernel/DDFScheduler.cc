static const char file_id[] = "DDFScheduler.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include "DDFScheduler.h"
#include "defConstructs.h"
#include "SDFScheduler.h"
	
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990,1991,1992,1993 The Regents of the University of California.
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
	
extern const char DDFdomainName[];

/*******************************************************************
		Main DDF scheduler routines
*******************************************************************/

extern void renewNumTokens(Block* b, int flag);

	////////////////////////////
	// setup
	////////////////////////////

void DDFScheduler :: setup () {
	// if candidateDomain is already set SDF, do SDFScheduling
	if (candidateDomain == SDF) {
		if (!galaxy()) {
			Error::abortRun("DDFScheduler: no galaxy!");
			return;
		}
		clearHalt();
		realSched->setup();
		return;
	}
	DynDFScheduler::setup();
}

void DDFScheduler::initStructures() {

	if (restructure && restructured == FALSE) {
		// fancy stuff...
		// auto-creation of SDF wormholes, decide the most efficient
		// scheduler (Case, For, DoWhile, Recur)
		makeSDFWormholes(*galaxy());
		selectScheduler(*galaxy());
		restructured = TRUE;
	}

	if (candidateDomain == DDF)
		DynDFScheduler::initStructures();
	Block* gpar = galaxy()->parent();
	if (gpar && gpar->isItWormhole()) 
		renewNumTokens(gpar, FALSE);
}

// my domain
const char* DDFScheduler :: domain () const { return DDFdomainName ;}

	////////////////////////////
	// selectScheduler
	////////////////////////////

void DDFScheduler :: selectScheduler(Galaxy& galaxy) {

	// select a candidate domain
	detectConstruct(galaxy);

	// set up the right scheduler
	switch(candidateDomain) {
		// special care for SDF domain.
	case SDF :
		LOG_NEW; realSched = new SDFScheduler;
		realSched->setGalaxy(galaxy);
		realSched->setup();
		if (galaxy.parent()) 
			renewNumTokens(galaxy.parent(), TRUE);
		break;
	case Unknown :
	case DDF : break;
	default :
		realSched = getScheduler(candidateDomain);
		if (!realSched) {
			Error::abortRun("Undefined Scheduler for ",
				nameType(candidateDomain), " construct.");
			return;
		}
		realSched->setGalaxy(galaxy);
		realSched->setup();
		break;
	}
}

	////////////////////////////
	// detectConstruct
	////////////////////////////

// This routine attempts to detect a special construct and run an
// optimized scheduler.  We cannot use this trick if there are wormholes
// for arbitrary domains, or stars that are not DDF, present.  This
// is a cheat and should be cleaned up.

void DDFScheduler :: detectConstruct(Galaxy& gal) {

	DFGalStarIter nextStar(gal);
	DataFlowStar* s;

	if ((candidateDomain == DDF) || (candidateDomain == SDF)) return;

	while ((s = nextStar++) != 0) {
		if (s->isItWormhole()) {
			const char* dom = s->scheduler()->domain();
			if (strcmp(dom , "DF")) {
				candidateDomain = DDF;
				return;
			}
		}
		else if (!s->isA("DDFStar")) {
			candidateDomain = DDF;
			return;
		}
	}

	candidateDomain = getType(gal);
}

// constructor: set default options
// By default, no restructuring is done, and a DDF scheduler is always used.
// To enable restrucuturing, you must call setParams.
//
DDFScheduler::DDFScheduler () {
	candidateDomain = DDF;
	restructure = FALSE;
	realSched = 0;
}

DDFScheduler::~DDFScheduler() { LOG_DEL; delete realSched; }

int DDFScheduler::isSDFType() {
	int flag = 1;
	if (candidateDomain == DDF) flag = 0;
	else if (candidateDomain == SDF) flag = 2;
	return flag;
}
