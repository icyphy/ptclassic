static const char file_id[] = "DataStruct.cc";
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  D. G. Messerschmitt, J. Buck
 Date of creation: 7/30/90

These are methods used by classes defined in DataStruct.h that
are too large to efficiently inline (formerly they were declared
inline anyway).

**************************************************************************/

#include "DataStruct.h"

void SingleLinkList :: insert(Pointer a)
{
	if (lastNode) {	// List not empty
		LOG_NEW; lastNode->next = new SingleLink(a,lastNode->next);
	}
	else	{	// List empty
		LOG_NEW; lastNode = new SingleLink(a,0);
		lastNode->next = lastNode;
	}
}

void SingleLinkList :: append(Pointer a)
{
	if (lastNode) {	// List not empty
		LOG_NEW; lastNode = lastNode->next = new SingleLink(a,lastNode->next);
	}
	else {		// List empty
		LOG_NEW; lastNode = new SingleLink(a,0);
		lastNode->next = lastNode;
	}
}

Pointer SingleLinkList :: getAndRemove()
{
	SingleLink *f = lastNode->next;	// Head of list
	Pointer r = f->e;

	if (f == lastNode) lastNode = 0; // List now empty
	else lastNode->next = f->next;

	LOG_DEL; delete f;
	return r;
}

Pointer SingleLinkList :: elem(int i) const
{
	SingleLink *f = lastNode->next;	// Head of list
	for( int t = i; t > 0; t-- )
		f = f->next;
	return f->e;
}

void SingleLinkList :: initialize()
{
	if (lastNode == 0) return;	// List already empty

	// Point to the first element in the list
	SingleLink *l = lastNode->next;

	// As long as the first element is not also the last, delete it
	while (l != lastNode ) {
		SingleLink *ll=l;
		l = l->next;
		LOG_DEL; delete ll;
	}

	// Delete the last node in the list
	LOG_DEL; delete lastNode;

	// and mark the list empty
	lastNode = 0;
}

// This function searches for an element in a SingleLinkList matching
// the argument, removing it if found.
int SingleLinkList::remove (Pointer x) {
	// case of empty list
	if (!lastNode) return 0;
	// case of 1-element list
	if (lastNode->next == lastNode) {
		if (lastNode->e != x) return 0;
		// only element matches, zero the list
		LOG_DEL; delete lastNode;
		lastNode = 0;
		return 1;
	}
	// general case
	SingleLink* f = lastNode->next;
	SingleLink* g = lastNode;
	do {
		if (f->e == x) {
			g->next = f->next;
			if (f == lastNode) lastNode = g;
			LOG_DEL; delete f;
			return 1;
		}
		f = f->next;
		g = g->next;
	} while (f != lastNode->next);
	return 0;
}

Pointer ListIter::next() {
	if (!ref) return 0;
	ref = ref->next;
	Pointer p = ref->e;
	if (ref == list->lastNode) ref = 0;
	return p;
}
