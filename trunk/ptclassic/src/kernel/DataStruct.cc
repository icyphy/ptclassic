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
#include <assert.h>

void SingleLinkList :: insert(Pointer a)
{
	if (!empty()) {	// List not empty
		LOG_NEW; lastNode->next = new SingleLink(a,lastNode->next);
	}
	else	{	// List empty
		LOG_NEW; lastNode = new SingleLink(a,0);
		lastNode->next = lastNode;
	}
}

void SingleLinkList :: append(Pointer a)
{
	if (!empty()) {	// List not empty
		LOG_NEW; lastNode = lastNode->next = new SingleLink(a,lastNode->next);
	}
	else {		// List empty
		LOG_NEW; lastNode = new SingleLink(a,0);
		lastNode->next = lastNode;
	}
}

Pointer SingleLinkList :: getAndRemove()
{
	// list must not be empty.
	assert (!empty());

	SingleLink *f = lastNode->next;	// Head of list
	Pointer r = f->e;

	if (f == lastNode) lastNode = 0; // List now empty
	else lastNode->next = f->next;

	LOG_DEL; delete f;
	return r;
}

Pointer SingleLinkList :: getTailAndRemove()
{
        // Note:  as in getAndRemove(), it is assumed that list is not empty
	assert (!empty());

        Pointer r = lastNode->e;        // Save contents of tail

        // Traverse list to find next-to-last node

        for( SingleLink*  p = lastNode;  p->next != lastNode;  p = p->next );

        if (p == lastNode) {            // If only one node in list, delete it
                LOG_DEL;  delete lastNode;
                lastNode = 0;           // List now empty
        }
        else  {                         // Else delete last node
                p->next = lastNode->next;
                LOG_DEL;  delete lastNode;
                lastNode = p;
        }
        return r;
}


Pointer SingleLinkList :: elem(int i) const
{
	if (empty()) return 0;
	SingleLink *f = lastNode->next;	// Head of list
	for( int t = i; t > 0; t-- )
		f = f->next;
	return f->e;
}

void SingleLinkList :: initialize()
{
	if (empty()) return;	// List already empty

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
	if (empty()) return 0;
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

// This function returns true if the argument is found in the list.
int SingleLinkList::member (Pointer x) const {
	if (empty()) return 0;
	SingleLink* f = lastNode;
	do {
		if (f->e == x) return TRUE;
		f = f->next;
	} while (f != lastNode);
	return FALSE;
}

Pointer ListIter::next() {
	if (!ref) return 0;
	ref = ref->next;
	Pointer p = ref->e;
	if (ref == list->lastNode) ref = 0;
	return p;
}
