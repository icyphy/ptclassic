/*****************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
                        All Rights Reserved.

Programmer: Soonhoi Ha

*****************************************************************/

#ifndef _CGSharedBus_h
#define _CGSharedBus_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "CGMultiTarget.h"
#include "UniProcessor.h"
#include "ParNode.h"

class CGSharedBus : public CGMultiTarget {
public:
	CGSharedBus(const char* name,const char* starType,const char* desc);

	void clearCommPattern();
	void saveCommPattern();
	void restoreCommPattern();

	// resource management
	int scheduleComm(ParNode*,int, int limit = 0);

	// For a given communication node, find a comm. node scheduled
	// just before the argument node on the same communication resource.
	ParNode* backComm(ParNode* n);

	Block* makeNew() const;

protected:
	UniProcessor bus;		// working bus used for scheduling
	UniProcessor bestBus;		// final bus.

	// redefine
	void resetResources();
};

#endif
