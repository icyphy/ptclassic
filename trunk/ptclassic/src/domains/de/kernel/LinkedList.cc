static const char file_id[] = "LinkedList.cc";
/**************************************************************************
Version identification:
@(#)LinkedList.cc	2.21	02/12/97

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
 Date of creation: 7/30/90

These are methods used by classes defined in DataStruct.h that
are too large to efficiently inline (formerly they were declared
inline anyway).

 Modified: John Davis, 5/18/97
        LinkedList class has been rewrittent to be more
        general. In particular, an arbitrary member can be
        removed (regardless of the position in the list) and
        the Links can be directly accessed in an efficient
        manner. 

**************************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "LinkedList.h"

Link::Link(Pointer a,Link* p) {
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

void Link::remove() {
    next->previous = previous;
    previous->next = next;
}

void LinkedList::directRemove( Link * a ) {
    Link* previous = a->previous;
    if( lastNode == a ) {
	 lastNode = a->previous;
    }
    a->remove();
    delete a;
    dimen--;
    if( dimen == 0 ) {
	 lastNode = 0;
	 previous = 0;
    }
    return;
}

Link* LinkedList::removeLink(Link& a) {
    Link* previous = a.previous;
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
LinkedList :: LinkedList(Pointer a) : dimen(1)
{
	LOG_NEW; lastNode= new Link(a,0);
}

// add at head of list
void LinkedList :: prepend(Pointer a)
{
	if (dimen > 0) {	// List not empty
	    LOG_NEW; new Link(a,lastNode->next);
	}
	else {	               // List empty
	    LOG_NEW; lastNode = new Link(a,0);
	}
	dimen++;
}

// add at tail of list
void LinkedList :: append(Pointer a)
{
	if (dimen > 0) {	// List not empty
	    LOG_NEW; lastNode = new Link(a,lastNode->next);
	} else {	        // List empty
	    LOG_NEW; lastNode = new Link(a,0);
	}
	dimen++;
}

Link * LinkedList :: appendGet(Pointer a)
{
        if (dimen > 0) {        // List not empty
            LOG_NEW;
            lastNode = new Link(a,lastNode->next);
        }
        else {                  // List empty
            LOG_NEW;
            lastNode = new Link(a,0);
        }
        dimen++;

        return lastNode;
}

// return and remove head of list
Pointer LinkedList :: getAndRemove()
{
	if (dimen == 0) return 0;

	Link *f = lastNode->next;	// Head of list
	Pointer r = f->e;

	removeLink(*f);
	
	return r;
}

// return and remove tail of list
Pointer LinkedList :: getTailAndRemove()
{
	if (dimen == 0) return 0;

        Pointer r = lastNode->e;        // Save contents of tail

	removeLink(*lastNode);
	
        return r;
}

// return i-th element of list, null if fewer than i elements.
Pointer LinkedList :: elem(int i) const
{
	if (i > dimen) return 0;
	Link *f = lastNode->next;	// Head of list
	for( int t = i; t > 0; t-- )
		f = f->next;
	return f->e;
}

void LinkedList :: initialize()
{
	if (empty()) return;	// List already empty

	// Point to the first element in the list
	Link *l = lastNode->next;

	// As long as the first element is not also the last, delete it
	while (l != lastNode ) {
		Link *ll=l;
		l = l->next;
		LOG_DEL; delete ll;
	}

	// Delete the last node in the list
	LOG_DEL; delete lastNode;

	// and mark the list empty
	lastNode = 0;
	dimen = 0;
}

// This function searches for an element in a LinkedList matching
// the argument, removing it if found.
int LinkedList::searchAndRemove (Pointer x) {
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
	Link* f = lastNode->next;
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
int LinkedList::member (Pointer x) const {
	if (dimen == 0) return 0;
	Link* f = lastNode;
	do {
		if (f->e == x) return TRUE;
		f = f->next;
	} while (f != lastNode);
	return FALSE;
}

void LinkedListIter::reset() {
    startAtHead = TRUE;
    ref = 0;
}

void LinkedListIter::reconnect(const LinkedList& l) {
    list = &l;
    reset();
}

void LinkedListIter::remove() {
    if (ref) {
        ref = ((LinkedList*)list)->removeLink(*ref);
        if (ref == list->lastNode) startAtHead = TRUE;
    }
    else if (list->lastNode)
        ((LinkedList*)list)->removeLink(*list->lastNode);
}

