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
	if (lastNode)	// List not empty
		lastNode->next = new SingleLink(a,lastNode->next);
	else	{	// List empty
		lastNode = new SingleLink(a,0);
		lastNode->next = lastNode;
		lastReference = lastNode;
	}
}

void SingleLinkList :: append(Pointer a)
{
	if (lastNode) 	// List not empty
		lastNode = lastNode->next = new SingleLink(a,lastNode->next);
	else {		// List empty
		lastNode = new SingleLink(a,0);
		lastNode->next = lastNode;
		lastReference = lastNode;
	}
}

Pointer SingleLinkList :: getAndRemove()
{
	SingleLink *f = lastNode->next;	// Head of list
	Pointer r = f->e;

	if (f == lastNode) lastNode = 0; // List now empty
	else lastNode->next = f->next;

	delete f;
	return r;
}

Pointer SingleLinkList :: elem(int i)
{
	SingleLink *f = lastNode->next;	// Head of list
	for( int t = i; t > 0; t-- )
		f = f->next;
	lastReference = f;
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
		delete ll;
	}

	// Delete the last node in the list
	delete lastNode;

	// and mark the list empty
	lastNode = 0;
}
