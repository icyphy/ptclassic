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

// add at head of list
void SequentialList :: prepend(Pointer a)
{
	if (dimen > 0) {	// List not empty
		LOG_NEW; lastNode->next = new SingleLink(a,lastNode->next);
	}
	else	{	// List empty
		LOG_NEW; lastNode = new SingleLink(a,0);
		lastNode->next = lastNode;
	}
	dimen++;
}

// add at tail of list
void SequentialList :: append(Pointer a)
{
	if (dimen > 0) {	// List not empty
		LOG_NEW; lastNode = lastNode->next = new SingleLink(a,lastNode->next);
	}
	else {		// List empty
		LOG_NEW; lastNode = new SingleLink(a,0);
		lastNode->next = lastNode;
	}
	dimen++;
}

// return and remove head of list
Pointer SequentialList :: getAndRemove()
{
	if (dimen == 0) return 0;

	SingleLink *f = lastNode->next;	// Head of list
	Pointer r = f->e;

	if (f == lastNode) lastNode = 0; // List now empty
	else lastNode->next = f->next;

	LOG_DEL; delete f;
	dimen--;
	return r;
}

// return and remove tail of list
Pointer SequentialList :: getTailAndRemove()
{
	if (dimen == 0) return 0;

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
	dimen--;
        return r;
}

// return i-th element of list, null if fewer than i elements.
Pointer SequentialList :: elem(int i) const
{
	if (i > dimen) return 0;
	SingleLink *f = lastNode->next;	// Head of list
	for( int t = i; t > 0; t-- )
		f = f->next;
	return f->e;
}

void SequentialList :: initialize()
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
	dimen = 0;
}

// This function searches for an element in a SequentialList matching
// the argument, removing it if found.
int SequentialList::remove (Pointer x) {
	// case of empty list
	if (dimen == 0) return 0;
	// case of 1-element list
	if (dimen == 1) {
		if (lastNode->e != x) return 0;
		// only element matches, zero the list
		LOG_DEL; delete lastNode;
		lastNode = 0;
		dimen = 0;
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
			dimen--;
			return 1;
		}
		f = f->next;
		g = g->next;
	} while (f != lastNode->next);
	return 0;
}

// This function returns true if the argument is found in the list.
int SequentialList::member (Pointer x) const {
	if (dimen == 0) return 0;
	SingleLink* f = lastNode;
	do {
		if (f->e == x) return TRUE;
		f = f->next;
	} while (f != lastNode);
	return FALSE;
}
