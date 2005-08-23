/**************************************************************************
Version identification:
@(#)DataStruct.h	2.19 12/9/97

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

class Queue
{
private:
	Pointer *buffer;	// expansible circular buffer for entries
	int bufferSize;		// allocated size of buffer array
	int numEntries;		// number of entries currently stored
	int firstEntry;		// array index of head entry

	void enlarge();		// make the buffer bigger

public:
	Queue();
	~Queue();

	void initialize();	// Remove all entries

	// Add element to the front or back of the queue
	void putHead(Pointer p);
	void putTail(Pointer p);
        void put(Pointer p) { putTail(p); }

	// Remove and return element from front or back of the queue;
	// these return 0 if the queue is empty
	Pointer getHead();
	Pointer getTail();
        Pointer get() { return getHead(); }

	Pointer head() const;	// get head element without removing it

	inline int size() const { return numEntries; }
	inline int length() const { return numEntries; } // alternate spelling
};

	///////////////////////////////////
	// class ListIter
	///////////////////////////////////

// ListIter steps sequentially through a SequentialList.  Warning: if
// the list is modified "out from under" the ListIter, bad things may
// happen if next() is called, though reset() will be safe.

class ListIter {
public:
	// reset to the beginning of a list
        void reset() {
	  startAtHead = TRUE;
	  ref = 0;
	}

	// constructor: attach to a SequentialList
	inline ListIter(const SequentialList& l) : list(&l) { reset(); }

	// next and operator++ are synonyms.  Return the next element,
	// return 0 if there are no more.
	// This routine has been re-implemented and optimized for speed
	// because of its heavy usage.  The if-structure is organized
	// so that necessary ifs are executed, but for cases where only
	// a few ifs are needed, the minimum number of ifs is done for
	// the most common cases, with rarer cases taking decreasing
	// priority in the if-structure.
	inline Pointer next() {
	  if (startAtHead) {
	    // Starting at the head of the list.  Use list->lastNode to find the
	    // head of the list.  If list->lastNode is NULL, the list is empty.
	    startAtHead = FALSE;
	    if (list->lastNode) {
	      ref = list->lastNode->next;
	      return ref->e;
	    }
	    // The list is empty, no entries:  passing the end of the list.
	    else {
	      ref = 0;
	      return ref;
	    }
	  }
	  else {
	    // Check to see if we're not at the end of the list.
	    if (ref != list->lastNode) {
              // Check to see if ref is not NULL:  end of list not yet passed.
	      if (ref) {
		ref = ref->next;
		return ref->e;
	      }
              // ref is NULL:  end of list was already passed.
	      else {
		return ref;
	      }
	    }
            // Passing the end of the list, no more entries.
	    else {
	      ref = 0;
	      return ref;
	    }
	  }
	}
    
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

class Stack : private Queue
{
public:
	Stack() {}

	// Add element at the top of the stack
	void pushTop(Pointer p) { putHead(p); }

	// Add element to the bottom of the stack
	void pushBottom(Pointer p) { putTail(p); }

	// Access and remove element from the top of the stack
	Pointer popTop() { return getHead(); }

	// Access but do not remove element from top of stack
	Pointer accessTop() const {return head();}

	Queue::initialize;
	Queue::size;

	// need destructor since baseclass is private, so others can destroy
	~Stack() {}
};

#endif
