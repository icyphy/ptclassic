/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. Buck

 Methods for InterpUniverse

 An InterpUniverse is an Universe with an interpreted galaxy
 in it.  This is accomplished by making it Runnable and an InterpGalaxy.
 This is the main object that interfaces talk to that makes Ptolemy
 work.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "InterpUniverse.h"

void InterpUniverse :: newSched() {
	delete scheduler;
	scheduler = KnownBlock::newSched();
	type = KnownBlock::domain();
}

Scheduler* InterpUniverse :: mySched() const {
	return scheduler;
}

// isa
ISA_FUNC(InterpUniverse,Galaxy);
