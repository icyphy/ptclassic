static const char file_id[] = "InterpUniverse.cc";
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
#include "KnownTarget.h"
#include "miscFuncs.h"

const char* InterpUniverse :: targetName() const {
	return targName ? targName : KnownTarget::defaultName(type);
}

int InterpUniverse :: newTarget(const char* newTargName) {
	if (target) {
		LOG_DEL; delete target;
		target = 0;
	}
	type = KnownBlock::domain();
	targName = newTargName;
	target = KnownTarget::clone (targetName());
	return target != 0;
}

Scheduler* InterpUniverse :: scheduler() const {
	return target->scheduler();
}

InterpUniverse :: ~InterpUniverse() {
	if (!target) return;
	LOG_DEL; delete target; target = 0;
}

// isa
ISA_FUNC(InterpUniverse,Galaxy);
