#ifndef _LinkedList_h
#define _LinkedList_h 1
/**************************************************************************
Version identification:
@(#)LinkedList.h	2.17	02/12/97

Copyright (c) 1990-1997 The Regents of the University of California.
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

 Programmer:  D. G. Messerschmitt, J. Buck and Jose Luis Pino
 Date of creation: 1/17/89

This header file contains basic container class data structures
used widely. Each container class stores a set of void*'s,
with different ways of accessing them. Normally, a data structure
is derived from one of these classes by adding conversion from
the type of interest to and from the void*, allocating and
deallocating memory for the objects, etc.

 Modified: John Davis, 5/18/97
        LinkedList class has been rewrittent to be more
        general. In particular, an arbitrary member can be
        removed (regardless of the position in the list) and
        the Links can be directly accessed in an efficient
        manner. The LinkedList class does not have an
        associated iterator.

**************************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "type.h"

	//////////////////////////////////////
	// class Link
	//////////////////////////////////////

class Link {
friend class LinkedList;
friend class ListIter;
protected:
    Link *next,*previous;
    Pointer e;
    Link(Pointer,Link*);
    void remove();
};

class LinkedList
{
	friend class ListIter;
public:
	// destructor
	~LinkedList()  { initialize(); }

	// constructor
	LinkedList() : lastNode(0), dimen(0) {}

	// constructor, with argument
	LinkedList(Pointer a);

	inline int size() const { return dimen;}
	
	void prepend(Pointer a);	// Add at head of list

	void append(Pointer a);	        // Add at tail of list

	void put(Pointer a) {append(a);} // synonym

	int remove(Pointer a);	// remove ptr: return TRUE if removed

	Pointer getAndRemove();	// Return and remove head of list

        Pointer getTailAndRemove();     // Return and remove tail of list

	inline Pointer head() const  // Return head, do not remove
	{
		return lastNode ? lastNode->next->e : 0;
	}
	
	inline Pointer tail() const {	// return tail, do not remove
		return lastNode ? lastNode->e : 0;
	}
	
	Pointer elem(int) const;// Return arbitary node of list

	void initialize();	// Remove all links

	// predicates
	// is list empty?
	inline int empty() const { return (lastNode == 0);}

	// is arg a member of the list? (returns TRUE/FALSE)
	int member(Pointer arg) const;

private:
	// remove a link from the list
        Link* removeLink(Link&);

        // Store head of list, so that there is a notion of 
        // first node on the list, lastNode->next is head of list 
        Link *lastNode;
	int dimen;
};


#endif
