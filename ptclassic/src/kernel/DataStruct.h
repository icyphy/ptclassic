/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

**************************************************************************/
#ifndef _DataStruct_h
#define _DataStruct_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "type.h"

	//////////////////////////////////////
	// class SingleLink
	//////////////////////////////////////

class SingleLink {
friend class SequentialList;
friend class ListIter;
protected:
    SingleLink *next,*previous;
    Pointer e;
    SingleLink(Pointer,SingleLink*);
    void remove();
};

class SequentialList
{
	friend class ListIter;
public:
	// destructor
	~SequentialList()  { initialize(); }

	// constructor
	SequentialList() : lastNode(0), dimen(0) {}

	// constructor, with argument
	SequentialList(Pointer a);

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
        SingleLink* removeLink(SingleLink&);

        // Store head of list, so that there is a notion of 
        // first node on the list, lastNode->next is head of list 
        SingleLink *lastNode;
	int dimen;
};


	/////////////////////////////////
	// class Queue
	/////////////////////////////////

/*
Class implements a queue, which may be used to implement FIFO or LIFO or a
mixture -- using putTail (or put), putHead, getTail, and getHead (or get).
*/

class Queue : private SequentialList
{
public:
	Queue() {}

	// Add element to the queue
        void put(Pointer p) {append(p);}
	void putTail(Pointer p) {append(p);}
	void putHead(Pointer p) {prepend(p);}

	// Remove and return element from end of the queue
        Pointer get() {return getAndRemove();}
	Pointer getHead() {return get();}
	Pointer getTail() {return getTailAndRemove();}

	SequentialList::size;
	SequentialList::initialize;

	inline int length() const { return size();} // TEMPORARY
	// need public destructor
	~Queue() {}
};

	///////////////////////////////////
	// class ListIter
	///////////////////////////////////

// ListIter steps sequentially through a SequentialList.  Warning: if
// the list is modified "out from under" the ListIter, bad things may
// happen if next() is called, though reset() will be safe.

class ListIter {
public:
	// constructor: attach to a SequentialList
	inline ListIter(const SequentialList& l) : list(&l) { reset(); }

	// reset to the beginning of a list
        void reset();

	// next and operator++ are synonyms.  Return the next element,
	// return 0 if there are no more.
        Pointer next();
    
	inline Pointer operator++ (POSTFIX_OP) { return next();}

        // attach the ListIter to a different object
	void reconnect(const SequentialList&);

        // remove the currently pointed to ref and update the
        // the ref pointer - if next hasn't been called, the lastNode
        // will be removed
        void remove();
private:
	const SequentialList* list;
	SingleLink *ref;
	int startAtHead;
};

	//////////////////////////////////////
	// class Stack
	//////////////////////////////////////

/*
Class implements a stack, where elements are added (pushed)
to the top of the stack or removed (pop'ed) from the top
of the stack. In addition, we allow elements to be added
to the bottom of the stack.
*/

class Stack : private SequentialList
{
public:
	Stack() {}

	// Add element at the top of the stack
	void pushTop(Pointer p) {prepend(p);}

	// Add element to the bottom of the stack
	void pushBottom(Pointer p) {append(p);}

	// Access and remove element from the top of the stack
	Pointer popTop() {return getAndRemove();}

	// Access but do not remove element from top of stack
	Pointer accessTop() const {return head();}

	SequentialList::initialize;
	SequentialList::size;

	// need destructor since baseclass is private, so others can destroy
	~Stack() {}
};

#endif
