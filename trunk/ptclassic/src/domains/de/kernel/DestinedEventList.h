#ifndef _DestinedEventList_h
#define _DestinedEventList_h 1
/**************************************************************************
Version identification:
$Id$

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
 Date: 5/19/97
	The DestinedEventList class is used to store CqLevelLinks
	of the Calendar Queue. Each DEStar has a DestinedEventList
	associated with it. When a star dies, its DestinedEventList
	removes pending events destined for the star.
**************************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "type.h"
#include "LinkedList.h"

class CqLevelLink;

class DestinedEventList : public LinkedList {
	friend class DestinedEventListIter;

public:
	// Constructor 
	DestinedEventList() : LinkedList() {}

	// Append to tail and return pointer to element
	Link * appendGet( CqLevelLink * obj ); 

	// Clear list
	// FIXME: This does not work since it does not 
	// remove the contents of the list. See page
	// 1-3 of the Kernel Manual
	void clearList() { LinkedList::initialize(); }

	// Remove an element from the list
	void remove( Link * obj ); 

	// Return the size of the list
	LinkedList::size;

};

class DestinedEventListIter : public LinkedListIter {
public:
        DestinedEventListIter(DestinedEventList& sl) : LinkedListIter(sl) {}
        inline DestinedEventList* next() 
		{ return (DestinedEventList*)LinkedListIter::next();}
        inline DestinedEventList* operator++(POSTFIX_OP) 
		{ return (DestinedEventList*)LinkedListIter::next();}
        LinkedListIter::reset;
        LinkedListIter::remove;
};

#endif




