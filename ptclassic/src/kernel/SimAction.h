/**************************************************************************
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
	SimAction* operator++(POSTFIX_OP) { return next();}
	ListIter::reset;
};


#endif
