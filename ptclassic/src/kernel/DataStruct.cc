static const char file_id[] = "DataStruct.cc";
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
 Date of creation: 7/30/90

These are methods used by classes defined in DataStruct.h that
are too large to efficiently inline (formerly they were declared
inline anyway).

**************************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "DataStruct.h"

SingleLink::SingleLink(Pointer a,SingleLink* p) {
    e=a;
    if (p) {
	next = p;
	previous = p->previous;
	p->previous = this;
	previous->next = this;
    }
    else {
	previous = this;
	next = this;
    }
}

void SingleLink::remove() {
    next->previous = previous;
    previous->next = next;
}

SingleLink* SequentialList::removeLink(SingleLink& a) {
    SingleLink* previous = a.previous;
    if (lastNode == &a) lastNode = a.previous;
    a.remove();
    delete &a;
    dimen--;
    if (dimen == 0) {
	lastNode = 0;
	previous = 0;
    }
    return previous;
}

// constructor with argument
SequentialList :: SequentialList(Pointer a) : dimen(1)
{
	LOG_NEW; lastNode= new SingleLink(a,0);
}

// add at head of list
void SequentialList :: prepend(Pointer a)
{
	if (dimen > 0) {	// List not empty
	    LOG_NEW; new SingleLink(a,lastNode->next);
	}
	else {	               // List empty
	    LOG_NEW; lastNode = new SingleLink(a,0);
	}
	dimen++;
}

// add at tail of list
void SequentialList :: append(Pointer a)
{
	if (dimen > 0) {	// List not empty
	    LOG_NEW; lastNode = new SingleLink(a,lastNode->next);
	}
	else {		        // List empty
	    LOG_NEW; lastNode = new SingleLink(a,0);
	}
	dimen++;
}

// return and remove head of list
Pointer SequentialList :: getAndRemove()
{
	if (dimen == 0) return 0;

	SingleLink *f = lastNode->next;	// Head of list
	Pointer r = f->e;

	removeLink(*f);
	
	return r;
}

// return and remove tail of list
Pointer SequentialList :: getTailAndRemove()
{
	if (dimen == 0) return 0;

        Pointer r = lastNode->e;        // Save contents of tail

	removeLink(*lastNode);
	
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
	do {
		if (f->e == x) {
		    removeLink(*f);
		    return 1;
		}
		f = f->next;
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

void ListIter::reset() {
    startAtHead = TRUE;
    ref = 0;
}

Pointer ListIter::next() {
    if (!startAtHead) {
	if (list->lastNode == 0) {
	  ref = 0;
	  return 0;
	}
	else if (ref == list->lastNode) {
	  ref = 0;
	  return 0;
	}
	else if (ref == 0) {
	  return 0;
	}
	else {
	  ref = ref->next;
	  return ref->e;
	}
    }
    else {
	startAtHead = FALSE;
	if (list->lastNode) {
	    ref = list->lastNode->next;
	    return ref->e;
	}
	else {
	    ref = 0;
	    return 0;
	}
    }
    return 0;
}

void ListIter::reconnect(const SequentialList& l) {
    list = &l;
    reset();
}

void ListIter::remove() {
    if (ref) {
	ref = ((SequentialList*)list)->removeLink(*ref);
	if (ref == list->lastNode) startAtHead = TRUE;
    }
    else if (list->lastNode)
	((SequentialList*)list)->removeLink(*list->lastNode);
}
