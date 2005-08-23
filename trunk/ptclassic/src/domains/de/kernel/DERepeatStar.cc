static const char file_id[] = "DERepeatStar.cc";
/******************************************************************
Version identification:
@(#)DERepeatStar.cc	2.17   12/04/97

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

 Programmer:  Soonhoi Ha

 DERepeatStar methods.

 These methods were formerly defined in DEStar.cc; split off
 5/23/91 by J. Buck

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "DERepeatStar.h"
#include "DEScheduler.h"
#include "DEWormhole.h"

// DERepeatStar constructor
DERepeatStar :: DERepeatStar() {
	feedbackIn = NULL;
	feedbackOut = NULL;
}

void DERepeatStar::initialize() {

	if (feedbackIn == NULL) {
		LOG_NEW; feedbackIn = new InDEPort;
		addPort(feedbackIn->setPort("feedbackIn", this));
		// hide the feedback connections from user interfaces
		feedbackIn->setAttributes(P_HIDDEN);
	}

	if (feedbackOut == NULL) {
		LOG_NEW; feedbackOut = new OutDEPort;
		addPort(feedbackOut->setPort("feedbackOut", this));
		feedbackOut->setAttributes(P_HIDDEN);
	}

	if (feedbackOut->far() == NULL)
	{
	    // make a feedback connection with a delay marker
	    feedbackOut->connect(*feedbackIn, 1);
	}

	// Do the rest of the initialization.
	DEStar::initialize();
}

// Destructor.
DERepeatStar::~DERepeatStar() {
	LOG_DEL; delete feedbackIn;
	LOG_DEL; delete feedbackOut;
}

void DERepeatStar :: refireAtTime(double when, double value /*= 0.0*/) {
	feedbackOut->put(when) << value;
}

int DERepeatStar :: canGetFired() {
	int i = feedbackIn->dataNew;
	if (i == TRUE)
		feedbackIn->dataNew = FALSE;	// reset the flag.
	return i;
}

// start method for DERepeatStar

void DERepeatStar :: begin() {
	Scheduler *mysched;
	// Need different access methods to the scheduler for
	// wormholes and for stars, unfortunately
	if (isItWormhole()) {
	  mysched = ((DEWormhole*)this)->outerSched();
	} else {
	  mysched = scheduler();
	}
	feedbackOut->put(mysched->now()) << 0.0;
}

// isA function
ISA_FUNC(DERepeatStar,DEStar);
 
void DERepeatStar::start(double time) {
       feedbackOut->put(time) << 1.0;
       feedbackOut->sendData();
}
