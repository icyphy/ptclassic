/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 5/30/90

*******************************************************************/
#include "DEStar.h"
#include "StringList.h"

/*******************************************************************

	class DEStar methods

********************************************************************/

// Redefine method setting internal data in the Block
// so that various DE-specific initilizations can be performed.
// If the parent pointer is not provied, it defaults to NULL
Block& DEStar :: setBlock(char* s, Block* parent = NULL) {
	// First invoke the generic setBlock
	Block::setBlock(s,parent);

	// Then, perform the DE specific initialization.
	completionTime = 0.0;
	arrivalTime = 0.0;

	return *this;
}

// DERepeatStar constructor

// we need a method (or constructor) to link the feedback connection.
// something like, feedbackIn.connect(feedbackOut, ..)

DERepeatStar :: DERepeatStar() {
	addPort(feedbackIn.setPort("feedbackIn", this));
	addPort(feedbackOut.setPort("feedbackOut", this));

	// make a feedback connection
	feedbackOut.connect(feedbackIn, 0);
}
