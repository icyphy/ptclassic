static const char file_id[] = "PriorityQueue.cc";
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY

 Programmer:  Soonhoi Ha
 Date of creation: 5/30/90

 The Priority Queue class.

**************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "PriorityQueue.h"

// Set the properties of the LevelLink class.
LevelLink* LevelLink :: setLink(Pointer a, double v, double fv, LevelLink* n,
			  LevelLink* b) {
	e = a;
	level = v;
	fineLevel = fv;
	next = n;
	before = b;
	return this;
}

// get a free LevelLink if any. If none, create a new LevelLink.
// Once created, it's never destroyed.
LevelLink* PriorityQueue :: getFreeLink() {
	LevelLink* temp;
	if (freeLinkHead) {
		temp = freeLinkHead;
		freeLinkHead = temp->next;
		numFreeLinks--;
	} else {
		LOG_NEW; temp = new LevelLink;
	}
	return temp;
}

// put an unsed LevelLink into the free List for later use.
void PriorityQueue :: putFreeLink(LevelLink* p) {
	p->next = freeLinkHead;
	freeLinkHead = p;
	numFreeLinks++;
}

// clear all free Links
void PriorityQueue :: clearFreeList() {
	if (!freeLinkHead) return;
	LevelLink* temp;
	while (freeLinkHead->next) {
		temp = freeLinkHead;
		freeLinkHead = freeLinkHead->next;
		LOG_DEL; delete temp;
	}
	LOG_DEL; delete freeLinkHead;
	numFreeLinks = 0;
}

// constructor
PriorityQueue :: PriorityQueue(Pointer a, double v, double fv = 1.0) :
	freeLinkHead(0), numberNodes(1), numFreeLinks(0)
{
	lastNode = getFreeLink()->setLink(a, v, fv, 0, 0);
	lastNode->next   = lastNode;
	lastNode->before = lastNode;
	lastReference = lastNode;
}

// lowest level first, lowest fineLevel first.

LevelLink* PriorityQueue :: levelput(Pointer a, double v, double fv)
{
	numberNodes++;			// increment numberNodes
	LevelLink* newLink = getFreeLink();

	if (lastNode == 0) {		// List is empty
		lastNode = newLink->setLink(a, v, fv, 0, 0);
		lastNode->next = lastNode;
		lastNode->before = lastNode;
		lastReference = lastNode;
		return newLink;
	} 

	LevelLink *l = lastNode;		// Tail of the queue

	// compare with lastNode first
	if (v > l->level || (v == l->level && fv >= l->fineLevel)) {
		lastNode = newLink->setLink(a,v,fv,l->next,l);
		l->next->before = lastNode;
		l->next = lastNode;
		lastReference = lastNode;
		return newLink;
	} 
	l = l->before;	
	while (l != lastNode) {			// compare but last
		if (v > l->level || (v == l->level && fv >= l->fineLevel)) {
		   LevelLink *tmp = newLink->setLink(a,v,fv,l->next,l);
		   l->next->before = l->next = tmp;
		   lastReference = lastNode;
		   return newLink;
		}
		l = l->before;
	}
	// at the top of the queue
	LevelLink *tmp = newLink->setLink(a,v,fv,l->next,l);
	l->next->before = l->next = tmp;
	lastReference = lastNode;
	return newLink;
}

// highest level first, highest fineLevel first.

LevelLink* PriorityQueue :: leveltup(Pointer a, double v, double fv)
{
	numberNodes++;			// increment numberNodes
	LevelLink* newLink = getFreeLink();

	if (lastNode == 0) {		// List is empty
		lastNode = newLink->setLink(a, v, fv, 0, 0);
		lastNode->next = lastNode;
		lastNode->before = lastNode;
		lastReference = lastNode;
		return newLink;
	} 

	LevelLink *l = lastNode->next;		// Head of the queue

	while (l != lastNode) {			// compare but last
		if (v < l->level || (v == l->level && fv < l->fineLevel)) {
		   LevelLink *tmp = newLink->setLink(a,v,fv,l,l->before);
		   l->before->next = l->before = tmp;
		   lastReference = lastNode;
		   return newLink;
		}
		l = l->next;
	}

	// compare with the lastNode last
	if (v > l->level || (v == l->level && fv >= l->fineLevel)) {
		lastNode = newLink->setLink(a,v,fv,l->next,l);
		l->next->before = lastNode;
		l->next = lastNode;
		lastReference = lastNode;
		return newLink;
	} 
	LevelLink *tmp = newLink->setLink(a,v,fv,l,l->before);
	l->before->next = l->before = tmp;
	lastReference = lastNode;
	return newLink;
}

void PriorityQueue :: pushBack(LevelLink* a) {
	if (!lastNode) {
		lastNode = a;
	} else {
		a->next = lastNode->next;
		lastNode->next->before = a;
	}
	a->before = lastNode;
	lastNode->next = a;
	lastReference = lastNode;
	numberNodes++;
}

void PriorityQueue :: initialize()
{
	if (lastNode == 0) return;		// Queue already empty

	// Point to the first element of the list
	LevelLink *l = lastNode->next;

	// Put all Links into the free List.
	while (l != lastNode) {
		LevelLink *ll = l;
		l = l->next;
		putFreeLink(ll);
	}

	// Delete the last node in the queue
	putFreeLink(lastNode);

	// and mark the queue empty
	lastNode = 0;
	numberNodes = 0;
}

// "get() and putFreeLink()"
Pointer PriorityQueue :: getFirstElem()
{
	LevelLink *f = get();
	putFreeLink(f);
	return f->e;
}

LevelLink* PriorityQueue :: get()
{
	LevelLink *f = lastNode->next;	// Head of list

	if (f == lastNode) lastNode = 0;	// List now empty
	else {
		lastNode->next = f->next;
		f->next->before = lastNode;
	}
	numberNodes--;
	return f;
}

// "teg() and putFreeLink()".
Pointer PriorityQueue :: getLastElem()
{
	LevelLink* f = teg();
	putFreeLink(f);
	return f->e;
}

LevelLink* PriorityQueue :: teg()
{
	LevelLink *f = lastNode;
	if (lastNode == lastNode->next) lastNode = 0;	// List now empty
	else {
		lastNode = f->before;
		lastNode->next = f->next;
		f->next->before = lastNode;
	}
	numberNodes--;
	return f;
}

void PriorityQueue :: extract(LevelLink* a)
{
	a->before->next = a->next;
	a->next->before = a->before;
	numberNodes--;

	if (a == lastNode) lastNode = a->before;
	if (numberNodes == 0) lastNode = 0;	// List now empty
	lastReference = a->before;

	// put it into free List.
	putFreeLink(a);
}

// Destructor -- delete all Links 
PriorityQueue :: ~PriorityQueue () {
	initialize();
	clearFreeList();
}
	
void PriorityQueue :: put (Pointer a, double v)

{
        // place at end of queue regardless of "number stamp" 

        numberNodes++;

        if (lastNode)  {
          // list not empty
	  LevelLink* tmp = getFreeLink()->setLink (a, v, 1.0, lastNode->next,
                                                 lastNode);
          lastNode->next = tmp;
          lastNode = tmp;
          lastNode->next->before = lastNode;
        }
        else  {
          // list empty
          lastNode = getFreeLink()->setLink (a, v, 1.0, 0, 0);
          lastNode->next = lastNode->before = lastNode;
        }
        lastReference = lastNode;
}

