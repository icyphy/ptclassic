static const char file_id[] = "DERepeatStar.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

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

// we need a method (or constructor) to link the feedback connection.
// something like, feedbackIn.connect(feedbackOut, ..)

DERepeatStar :: DERepeatStar() {
	addPort(feedbackIn.setPort("feedbackIn", this));
	addPort(feedbackOut.setPort("feedbackOut", this));

	// make a feedback connection
	feedbackOut.connect(feedbackIn, 0);
	feedbackIn.triggers();
	feedbackIn.depth = MINDEPTH;

	// hide the feedback connections from user interfaces
	feedbackOut.setAttributes(P_HIDDEN);
	feedbackIn.setAttributes(P_HIDDEN);
}

void DERepeatStar :: refireAtTime(double when) {
	feedbackOut.put(when) << 0.0;
}

int DERepeatStar :: canGetFired() {
	int i = feedbackIn.dataNew;
	if (i == TRUE)
		feedbackIn.dataNew = FALSE;	// reset the flag.
	return i;
}

// start method for DERepeatStar

void DERepeatStar :: initialize() {
	DEStar :: initialize();
	feedbackOut.put(completionTime) << 0.0;
}

