/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 5/30/90

 The Priority Queue class.

**************************************************************************/

#include "PriorityQueue.h"

// Set the properties of the LevelLink class.
LevelLink* LevelLink :: setLink(Pointer a, float v, float fv, LevelLink* n,
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
		temp = new LevelLink;
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
		delete temp;
	}
	delete freeLinkHead;
	numFreeLinks = 0;
}

// constructor
PriorityQueue :: PriorityQueue(Pointer a, float v, float fv = 1.0) :
	freeLinkHead(0), numberNodes(1), numFreeLinks(0)
{
	lastNode = getFreeLink()->setLink(a, v, fv, 0, 0);
	lastNode->next   = lastNode;
	lastNode->before = lastNode;
	lastReference = lastNode;
}

// lowest level first, lowest fineLevel first.

void PriorityQueue :: levelput(Pointer a, float v, float fv = 1.0)
{
	numberNodes++;			// increment numberNodes

	if (lastNode == 0) {		// List is empty
		lastNode = getFreeLink()->setLink(a, v, fv, 0, 0);
		lastNode->next = lastNode;
		lastNode->before = lastNode;
		lastReference = lastNode;
		return;
	} 

	LevelLink *l = lastNode;		// Tail of the queue

	// compare with lastNode first
	if (v > l->level || (v == l->level && fv >= l->fineLevel)) {
		lastNode = getFreeLink()->setLink(a,v,fv,l->next,l);
		l->next->before = lastNode;
		l->next = lastNode;
		lastReference = lastNode;
		return;
	} 
	l = l->before;	
	while (l != lastNode) {			// compare but last
		if (v > l->level || (v == l->level && fv >= l->fineLevel)) {
		   l->next->before = l->next = 
				getFreeLink()->setLink(a,v,fv,l->next,l);
		   lastReference = lastNode;
		   return;
		}
		l = l->before;
	}
	// at the top of the queue
	l->next->before = l->next = getFreeLink()->setLink(a,v,fv,l->next,l);
	lastReference = lastNode;
	return;
}

void PriorityQueue :: pushBack(LevelLink* a) {
	if (!lastNode) {
		lastNode = a;
	} else {
		a->next = lastNode->next;
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
	
