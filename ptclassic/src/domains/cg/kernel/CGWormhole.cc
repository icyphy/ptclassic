static const char file_id[] = "CGWormhole.cc";

/******************************************************************
Version identification:
 $Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: Soonhoi Ha 
 Date of creation: 6/15/90
 Date of revision: 6/20/90

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGWormhole.h"
#include "GalIter.h"
#include "StringList.h"
#include "Error.h"
#include "Scheduler.h"

/*******************************************************************

	class CGWormhole methods

********************************************************************/

void CGWormhole :: start() {

	// check whether the inner domain is supported. 
	// If not, currently error.
	StringListIter domName(supportedDomains);
	const char* n;
	int flag = FALSE;
	while ((n = domName++) != 0)
		if (!strcmp(insideDomain(), n)) {
			flag = TRUE;
		}
	if (flag == FALSE) {
		Error::abortRun("CGWormhole is not defined with ", 
			insideDomain(), " domain inside.");
		return;
	}
	
	// get the number of processors.
	assignProcs(targetPtr->nProcs());

	if (mtarget) {
		// inform the inner domain, how many processors are available.
		mtarget->setTargets(numProcs);
		mtarget->initState();

		// Inherit the outside child targets to inside if necessary.
		if (mtarget->inherited()) {
			if (!mtarget->inheritChildTargets(targetPtr)) return;
		}

		// Copy the pointer of the profile to the inside.
		mtarget->setProfile(profile);

	}

	// common function
	Wormhole :: setup();
	
	if (mtarget) {
		// set its average execution time
		execTime = mtarget->totalWorkLoad();
	}
}

void CGWormhole :: go() {
	// set the currentTime of the inner domain.
	target->setCurrentTime(targetPtr->mySched()->currentTime);

	// run
	run();
}

CGWormhole* CGWormhole :: myWormhole() {
	return this;
}

/**********************************************
  Make the inside galaxy ready for scheduling.
 **********************************************/

// Constructor
CGWormhole :: CGWormhole(Galaxy& g, Target* t) : Wormhole(*this,g,t),mtarget(0)
{
	buildEventHorizons ();
	profile = (Profile *) 0;
	prevNum = numProcs = 0;

	// initialize supportedDomains.
	supportedDomains.initialize();
	supportedDomains += "CGDDF";
	supportedDomains += "CG";
}

CGWormhole :: ~CGWormhole() {
	DEL_ARRAY(profile,prevNum);
	freeContents();
}

StringList CGWormhole :: printVerbose() const {
	return Wormhole :: print(0);
}

StringList CGWormhole :: printRecursive() const {
	return Wormhole :: print(1);
}

// Display schedules
StringList CGWormhole :: displaySchedule() {
	return target->displaySchedule();
}

// compute schedule
void CGWormhole :: insideSchedule() {
	if (!mtarget) return;
	if (optNum != numProcs) mtarget->setTargets(optNum);
	mtarget->insideSchedule();
}
	
// download the code
void CGWormhole :: downLoadCode(int index) {
	if (!mtarget) return;
	if (mtarget->inherited())
	   mtarget->setCurChild(((BaseMultiTarget*)targetPtr)->getCurChild());
	mtarget->downLoadCode(index, profile + numProcs - 1);
}
	
// cloner -- clone the inside and make a new wormhole from that.
Block* CGWormhole :: clone() const {
	LOG_NEW; return new CGWormhole(gal.clone()->asGalaxy(), target->cloneTarget());
}

/******************************************************************
		Methods for parallel scheduler
 *****************************************************************/

void CGWormhole :: assignProcs(int num) {
	// set-up the processors
	if (num <= 0) {
		StringList out;
		out += "neg. number of processors are requested.";
	  	Error::abortRun(*this, out);
	} else if (numProcs == num) {
		for (int i = 0; i < numProcs; i++)
			profile[i].initialize();
	} else {
		DEL_ARRAY(profile,prevNum);
		prevNum = numProcs = num;
		LOG_NEW; profile = new Profile[numProcs];
		for (int i = 0; i < numProcs; i++) {
			profile[i].create(i+1);
		}
	}
	optNum = numProcs;

	// When the system is under MultiProcessor target, define "mtarget".
	// A kind of heckery.
	if (targetPtr->child(0)) mtarget = (BaseMultiTarget*) target;
}

// calculate the optimal number of assigned processors.
void CGWormhole :: computeProfile(int num, int resWork, IntArray* avail)
{
	if (!mtarget) return;
	numProcs = num;
	optNum = mtarget->computeProfile(num, resWork, avail);
}

