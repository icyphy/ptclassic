#ifndef _DataStruct_h
#define _DataStruct_h 1

#include "type.h"

/**************************************************************
This header file contains basic container class data structures
used widely. Each container class stores a set of void*'s,
with different ways of accessing them. Normally, a data structure
is derived from one of these classes by adding conversion from
the type of interest to and from the void*, allocating and
deallocating memory for the objects, etc.

Programmer: D.G. Messerschmitt
	    U.C. Berkeley
Date: Jan 10, 1990
**************************************************************/

	//////////////////////////////////////
	// class SingleLink
	//////////////////////////////////////

class SingleLink
{
	friend class SingleLinkList;

	SingleLink *next;
	Pointer e;

	SingleLink(Pointer a, SingleLink* p) {e=a;next=p;}
};

class SingleLinkList
{
protected:

	void insert(Pointer a);	// Add at head of list
	void append(Pointer a);	// Add at tail of list
	Pointer getAndRemove();	// Return and remove head of list
	Pointer next();		// Return next node on list, relative to
				// last reference
	Pointer elem(int);	// Return arbitary node of list
	void clear();		// Remove all links

        // Reset the last reference pointer so that accesses start
        // at the head of the list
        void reset() { lastReference = lastNode; }

	SingleLinkList() { lastNode = 0; }
	SingleLinkList(Pointer);
	~SingleLinkList()  { clear(); }

private:

        // Store head of list, so that there is a notion of 
        // first node on the list, lastNode->next is head of list 
        SingleLink *lastNode;
 
        // Store last access to the list, so we can sequentially 
        // access the list
        SingleLink *lastReference;
};

inline SingleLinkList :: SingleLinkList(Pointer a)
{
	lastNode= new SingleLink(a,0);
	lastNode->next=lastNode;
	lastReference = lastNode;
}

inline void SingleLinkList :: insert(Pointer a)
{
	if (lastNode)	// List not empty

		lastNode->next = new SingleLink(a,lastNode->next);

	else	{	// List empty

		lastNode = new SingleLink(a,0);
		lastNode->next = lastNode;
		lastReference = lastNode;
		}
}
inline void SingleLinkList :: append(Pointer a)
{
	if (lastNode) 	// List not empty

		lastNode = lastNode->next = new SingleLink(a,lastNode->next);

	else {		// List empty

		lastNode = new SingleLink(a,0);
		lastNode->next = lastNode;
		lastReference = lastNode;
		}
}
inline Pointer SingleLinkList :: getAndRemove()
{
	SingleLink *f = lastNode->next;	// Head of list
	Pointer r = f->e;

	if (f == lastNode) lastNode = 0; // List now empty
	else lastNode->next = f->next;

	delete f;
	return r;
}

inline Pointer SingleLinkList :: next()
{
	lastReference = lastReference->next;
	return lastReference->e;
}

inline Pointer SingleLinkList :: elem(int i)
{
	SingleLink *f = lastNode->next;	// Head of list
	for( int t = i; t > 0; t-- )
		f = f->next;
	lastReference = f;
	return f->e;
}

inline void SingleLinkList :: clear()
{
	SingleLink *l = lastNode;
	if (l == 0) return;	// List already empty

	do	{

		SingleLink *ll=l;
		l = l->next;
		delete ll;

		} while (l != lastNode );
}

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
protected:
	// Add element to the vector
        void put(Pointer p) {append(p); ++dimen;}

	// Return the dimension of the vector
	int dimension() {return dimen;}

	// Return and element of the vector, given index
	Pointer elem (int index);

	Vector() {dimen = 0;}

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
        void put(Pointer); 	// Add to the queue
        Pointer get();		// Remove and return from the queue
	int length();		// Return the length of the queue
        void clear();		// Clear the queue
	Queue();
};

inline void Queue :: put(Pointer p)
	{ ++numberNodes; SingleLinkList::append(p); }

inline Pointer Queue :: get()
	{ --numberNodes; return getAndRemove(); }

inline int Queue :: length()
	{ return numberNodes; }

inline void Queue ::  clear()
	{ SingleLinkList::clear(); }

inline Queue :: Queue()
	{ numberNodes = 0; }

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
protected:
	// Add element to the list
        void put(Pointer p) {append(p); ++dimen;}

	// Return number of elements on the list
	int size() {return dimen;}

	// Return next element on the list
	Pointer next() {SingleLinkList::next();}

	// Reset, so that the next access will be the
	// first element that was added to the list
	void reset() {SingleLinkList::reset();}

	SequentialList() {dimen=0;}

private:
	int dimen;	// Size of list
};

#endif
