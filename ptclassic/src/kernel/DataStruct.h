/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  D. G. Messerschmitt
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

#include "type.h"


	//////////////////////////////////////
	// class SingleLink
	//////////////////////////////////////

class SingleLink
{
	friend class SingleLinkList;
	friend class ListIter;

	SingleLink *next;
	Pointer e;

	SingleLink(Pointer a, SingleLink* p) {e=a;next=p;}
};

class SingleLinkList
{
public:
	// destructor
	~SingleLinkList()  { initialize(); }
	// constructor
	SingleLinkList() { lastNode = 0; }
	// constructor, with argument
	SingleLinkList(Pointer a)
	{
		lastNode= new SingleLink(a,0);
		lastNode->next=lastNode;
	}

	void insert(Pointer a);	// Add at head of list
	void append(Pointer a);	// Add at tail of list
	int remove(Pointer a);	// remove ptr: return TRUE if removed
	Pointer getAndRemove();	// Return and remove head of list
	Pointer getNotRemove()  // Return head, do not remove
	{
		return lastNode->next->e;
	}
	Pointer elem(int) const;// Return arbitary node of list
	void initialize();	// Remove all links

protected:

        // Store head of list, so that there is a notion of 
        // first node on the list, lastNode->next is head of list 
        SingleLink *lastNode;
};


	/////////////////////////////////////
	// class Vector
	////////////////////////////////////

/*
Class implements a vector which is accessed by an index and can
 grow as elements are added (it never shrinks)
Each element of the vector is a Pointer....so arbitrary objects
 can be stored in the vector
*/

class Vector : SingleLinkList
{
public:
	// Add element to the vector
        void put(Pointer p) {append(p); ++dimen;}

	// Return the dimension of the vector
	int dimension() const {return dimen;}

	// Return and element of the vector, given index
	Pointer elem (int index);

	// Clear out the data structure
	void initialize() {SingleLinkList::initialize(); dimen=0;}

	Vector() {dimen = 0;}

	// need public destructor
	~Vector() {}

private:
	int dimen;	// Size of vector
};

inline Pointer Vector :: elem (int i)
	{
	// No error checking on index at present
	// if(i >= dimen) error
	return SingleLinkList :: elem(i);
	}

	/////////////////////////////////
	// class Queue
	/////////////////////////////////

/*
Class implements a queue or first-in first-out buffer
*/

class Queue : SingleLinkList
{
	int numberNodes;
public:
	// Add element to the queue
        void put(Pointer p) {++numberNodes; SingleLinkList::append(p);}

	// Remove and return element from end of the queue
        Pointer get() {--numberNodes; return getAndRemove();}

	// Return number of elements currently in queue
	int length() const {return numberNodes;}

	// Clear the queue
        void initialize() {SingleLinkList::initialize(); numberNodes=0;}

	Queue() {numberNodes = 0;}

	// need public destructor
	~Queue() {}
};


	/////////////////////////////////////
	//  class SequentialList
	/////////////////////////////////////

/*
This is a container class containing a list of elements
that are added and accessed sequentially; it is optimized for
those situations where a list of elements is needed and access
is sequential through the list

It is recommended that operator ++ be used
to access the next element of the list
*/

class SequentialList : SingleLinkList
{
	friend class ListIter;
public:
	// Add element to the end of the list
        void put(Pointer p) {append(p); ++dimen;}

	// Add element to the beginning of the list
        void tup(Pointer p) {insert(p); ++dimen;}

	// Return number of elements on the list
	int size() const {return dimen;}

	// Return head of list
	Pointer head() const { return elem(0);}

	// Remove an object from the list
	int remove(Pointer p) {
		int i = SingleLinkList::remove(p);
		dimen -= i;
		return i;
	}

	// Clear the data structure
	void initialize() {SingleLinkList::initialize(); dimen=0;}

	SequentialList() {dimen=0;}

	// need destructor since baseclass is private, so others can destroy
	~SequentialList() {}
private:
	int dimen;	// Size of list
};

	///////////////////////////////////
	// class ListIter
	///////////////////////////////////

class ListIter {
public:
	ListIter(const SequentialList& l) : list(&l), ref(l.lastNode) {}
	reset() { ref = list->lastNode;}
	Pointer next() {
		if (!ref) return 0;
		ref = ref->next;
		Pointer p = ref->e;
		if (ref == list->lastNode) ref = 0;
		return p;
	}
	Pointer operator++ () { return next();}
protected:
// attach the ListIter to a different object
	void reconnect(const SequentialList& l) {
		list = &l; ref = l.lastNode;
	}
private:
	const SequentialList* list;
	SingleLink *ref;
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

class Stack : SingleLinkList
{
public:
	// Add element at the top of the stack
	void pushTop(Pointer p) {insert(p);++dimen;}

	// Add element to the bottom of the stack
	void pushBottom(Pointer p) {append(p);++dimen;}

	// Access and remove element from the top of the stack
	Pointer popTop() {--dimen; return getAndRemove();}

	// Access but do not remove element from top of stack
	Pointer accessTop() {return getNotRemove();}

	// Clear the data structure
	void initialize() {SingleLinkList::initialize(); dimen=0;}

	int size() const {return dimen;}

	Stack() {dimen=0;}

	// need destructor since baseclass is private, so others can destroy
	~Stack() {}
private:
	int dimen;	// Number elements on the stack
};

#endif
