static const char file_id[] = "CGWormBase.cc";

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
 Date of creation: 2/23/93

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGWormBase.h"
#include "GalIter.h"
#include "Error.h"
#include "Scheduler.h"
#include "SimControl.h"

/*******************************************************************

	class CGWormBase methods

********************************************************************/

void CGWormBase :: setup() {

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
		Error::abortRun("CGWormBase is not defined with ", 
			insideDomain(), " domain inside.");
		return;
	}
	
	assignProcs(outerTarget->nProcs());

	if (mtarget) {
		// inform the inner domain, how many processors are available.
		mtarget->setTargets(numProcs);
		mtarget->initState();

		// Inherit the outside child targets to inside if necessary.
		if (mtarget->inherited()) {
			if (!mtarget->inheritChildTargets(outerTarget)) return;
		}
	}

	// common setup function, and error checking.
	Wormhole :: setup();
	if (SimControl::haltRequested()) return;

	if (mtarget && outerTarget->child(0)) {
		// Copy the pointer of the profile to the inside.
		mtarget->setProfile(myProfile);

		// set its average execution time
		execTime = mtarget->totalWorkLoad();
	}
}

/**********************************************
  Make the inside galaxy ready for scheduling.
 **********************************************/

// Constructor
CGWormBase :: CGWormBase(Star& s, Galaxy& g, Target* t) : 
	Wormhole(s,g,t),mtarget(0)
{
	myProfile = (Profile *) 0;
	prevNum = numProcs = 0;

	// initialize supportedDomains.
	supportedDomains.initialize();
	supportedDomains += "CGDDF";
	supportedDomains += "CG";

	// selfWorm
	selfWorm = this;
}

CGWormBase :: ~CGWormBase() {
	DEL_ARRAY(myProfile,prevNum);
}

// return the profile
Profile* CGWormBase :: getProfile(int pNum) {
	return &myProfile[pNum - 1];
}

// Display schedules
StringList CGWormBase :: displaySchedule() {
	return target->displaySchedule();
}

// compute schedule
int CGWormBase :: insideSchedule() {
	if (!mtarget) return TRUE;
	if (optNum != numProcs) mtarget->setTargets(optNum);
	return mtarget->insideSchedule(myProfile + numProcs - 1);
}
	
// download the code
void CGWormBase :: downLoadCode(int invoc, int pix) {
	if (!mtarget) return;
	mtarget->downLoadCode(invoc, pix, myProfile + numProcs - 1);
}
	
/******************************************************************
		Methods for parallel scheduler
 *****************************************************************/

void CGWormBase :: assignProcs(int num) {
	// set-up the processors
	if (num <= 0) {
		StringList out;
		out += "neg. number of processors are requested.";
	  	Error::abortRun(out);
	} else if (numProcs == num) {
		for (int i = 0; i < numProcs; i++)
			myProfile[i].initialize();
	} else {
		DEL_ARRAY(myProfile,prevNum);
		prevNum = numProcs = num;
		LOG_NEW; myProfile = new Profile[numProcs];
		for (int i = 0; i < numProcs; i++) {
			myProfile[i].create(i+1);
		}
	}
	optNum = numProcs;

	// When the system is under MultiProcessor target, define "mtarget".
	// A hack.
	if (target->isA("MultiTarget")) mtarget = (MultiTarget*) target;
}

// set up profiles -> create assignedId member of profile
void CGWormBase :: setupProfile(int n) {
	for (int i = 0; i < numProcs; i++)
		myProfile[i].createAssignArray(n);
}

// calculate the optimal number of assigned processors.
void CGWormBase :: computeProfile(int num, int resWork, IntArray* avail)
{
	if (!mtarget) return;
	numProcs = num;
	optNum = mtarget->computeProfile(num, resWork, avail);
}

// perform the manual scheduling of the inside domain
Profile* CGWormBase :: manualSchedule(int count) {
	if (!mtarget) return 0;
	return mtarget->manualSchedule(count);
}
