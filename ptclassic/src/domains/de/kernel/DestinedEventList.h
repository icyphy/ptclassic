#ifdef _DestinedEventList_h
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

class DestinedEventList : public LinkedList {
	// friend class DEStar;

public:
	// Constructor 
	DestinedEventList() : lastNode(0), dimen(0) { intialize(); }

	// Return the size of the list
	LinkedList::size;

	// Remove an element from the list
	int remove( Link * obj ) { return LinkedList::removeLink( obj ); }

	// Append to tail and return pointer to element
	Link * appendGet( Link * obj ) { return LinkedList::appendGet( obj ); }

	// Clear list
	void clearList() { LinkedList::initialize(); }

};

#endif


