/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck
 Date of creation: 6/23/92

SimAction is used internally by the SimControl code.  A SimAction
represents an action to be called under certain conditions.

**************************************************************************/
#ifndef _SimAction_h
#define _SimAction_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "DataStruct.h"
#include "SimControl.h"

class Star;

class SimAction {
	SimActionFunction func;
	const char* arg;
	Star* which;
public:
	SimAction(SimActionFunction f, const char* text = 0, Star* star = 0)
		: func(f), arg(text), which(star) {}
	int match (Star* star) const {
		return (which == 0 || which == star);
	}
	void apply (Star* star) {
		func(star, arg);
	}
};

class SimActionList : private SequentialList {
	friend class SimActionListIter;
public:
	SimActionList();
	~SimActionList();
	void put(SimAction* a) { SequentialList::put(a);}
	SimAction* head() const { return (SimAction*)SequentialList::head();}
	int member(SimAction* a) { return SequentialList::member(a);}
	int remove(SimAction* a) { return SequentialList::remove(a);}
};

class SimActionListIter : private ListIter {
public:
	SimActionListIter(SimActionList& alist) : ListIter(alist) {}
	SimAction* next() { return (SimAction*)ListIter::next();}
	SimAction* operator++() { return next();}
	ListIter::reset;
};


#endif
