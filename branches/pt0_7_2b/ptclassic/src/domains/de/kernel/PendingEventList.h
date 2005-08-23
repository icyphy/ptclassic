#ifndef _PendingEventList_h
#define _PendingEventList_h 1
/**************************************************************************
Version identification:
@(#)PendingEventList.h	1.8 01/29/98

Copyright (c) 1997 The Regents of the University of California.
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

 Programmer: John Davis
 Date: 12/15/97
	The PendingEventList class is used to store CqLevelLinks
	of the Calendar Queue. Each DEStar which has its starIsMutable
	parameter set to true has a PendingEventList associated with 
	it. When a star dies, its PendingEventList removes pending 
	events destined for the star.
**************************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "type.h"
#include "LinkedList.h"

class MutableCQEventQueue;
class CqLevelLink;
class DEStar;


class PendingEventList : public LinkedList {
	friend class DEStar;
	friend class PendingEventListIter;

public:
	// Constructor 
	PendingEventList(MutableCQEventQueue *eventQueue); 

	// Destructor
	~PendingEventList();

	// Append to tail and return pointer to element
	Link * appendGet(CqLevelLink * obj); 

	// return and remove head of list
	void removeHeadAndFree(); 

	// Free a CqLevelLink. Note that this function requires
	// the destinationRef to be set to 0 before being called.
	// Otherwise an error will occur.
	void freeEvent(CqLevelLink * obj);

	// Clear list
	LinkedList::initialize;

	// Return the size of the list
	LinkedList::size;

private:
        // Return and remove head of list
        CqLevelLink * getHeadAndRemove() {
	    return (CqLevelLink *)LinkedList::getHeadAndRemove();
	}

	// Remove an element from the list
	CqLevelLink * remove( Link * obj ); 

	// The Event Queue owned by this PendingEventList
	MutableCQEventQueue *myQueue;

};



        ///////////////////////////////////
        // class PendingEventListIter
        ///////////////////////////////////

// PendingEventListIter steps sequentially through a PendingEventList.  
// Warning: if the list is modified "out from under" the 
// PendingEventListIter, bad things may happen if next() is called, 
// though reset() will be safe.

class PendingEventListIter : public LinkedListIter {
public:
        // constructor: attach to a PendingEventList
        inline PendingEventListIter(PendingEventList& l) 
		: LinkedListIter(&l) { }

        // next and operator++ are synonyms.  Return the next element,
        // return 0 if there are no more.
        // This routine has been re-implemented and optimized for speed
        // because of its heavy usage.  The if-structure is organized
        // so that necessary ifs are executed, but for cases where only
        // a few ifs are needed, the minimum number of ifs is done for
        // the most common cases, with rarer cases taking decreasing
        // priority in the if-structure.
	inline CqLevelLink* next() { 
	    // FIXME: myQueue->decrementEventCount();
	    return (CqLevelLink*)LinkedListIter::next();
	}
        inline CqLevelLink* operator++ (POSTFIX_OP) { return next();}

        LinkedListIter::reset;
	LinkedListIter::remove;
};


#endif


