static const char file_id[] = "DERepeatStar.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

// DERepeatStar constructor
DERepeatStar :: DERepeatStar() {
	feedbackIn = NULL;
	feedbackOut = NULL;
}

void DERepeatStar::initialize() {

	if (feedbackIn == NULL) {
		LOG_NEW; feedbackIn = new InDEPort;
		addPort(feedbackIn->setPort("feedbackIn", this));
	}

	if (feedbackOut == NULL) {
		LOG_NEW; feedbackOut = new OutDEPort;
		addPort(feedbackOut->setPort("feedbackOut", this));
	}

	if (feedbackOut->far() == NULL)
	{
	    // make a feedback connection
	    feedbackOut->connect(*feedbackIn, 0);
	    feedbackIn->triggers();
	    feedbackIn->depth = MINDEPTH;

	    // hide the feedback connections from user interfaces
	    feedbackOut->setAttributes(P_HIDDEN);
	    feedbackIn->setAttributes(P_HIDDEN);
	}

	// Do the rest of the initialization.
	DEStar::initialize();
}

// Destructor.
DERepeatStar::~DERepeatStar() {
	LOG_DEL; delete feedbackIn;
	LOG_DEL; delete feedbackOut;
}

void DERepeatStar :: refireAtTime(double when) {
	feedbackOut->put(when) << 0.0;
}

int DERepeatStar :: canGetFired() {
	int i = feedbackIn->dataNew;
	if (i == TRUE)
		feedbackIn->dataNew = FALSE;	// reset the flag.
	return i;
}

// start method for DERepeatStar

void DERepeatStar :: begin() {
	feedbackOut->put(completionTime) << 0.0;
}

