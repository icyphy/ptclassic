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

PriorityQueue :: PriorityQueue(Pointer a, float v)
{
	lastNode = new LevelLink(a, v, 0, 0);
	lastNode->next   = lastNode;
	lastNode->before = lastNode;
	lastReference = lastNode;
	numberNodes = 1;
}

void PriorityQueue :: levelput(Pointer a, float v)
{
	numberNodes++;			// increment numberNodes

	if (lastNode == 0) {		// List is empty
		lastNode = new LevelLink(a, v, 0, 0);
		lastNode->next = lastNode;
		lastNode->before = lastNode;
		lastReference = lastNode;
		return;
	} 

	LevelLink *l = lastNode;		// Tail of the queue

	// compare with lastNode first
	if (v >= l->level) {
		lastNode = new LevelLink(a,v,l->next,l);
		l->next->before = lastNode;
		l->next = lastNode;
		lastReference = lastNode;
		return;
	} 
	l = l->before;	
	while (l != lastNode) {			// compare but last
		if (v >= l->level) {
		   l->next->before = l->next = 
				new LevelLink(a,v,l->next,l);
		   lastReference = lastNode;
		   return;
		}
		l = l->before;
	}
	// at the top of the queue
	l->next->before = l->next = new LevelLink(a,v,l->next,l);
	lastReference = lastNode;
	return;
}

void PriorityQueue :: initialize()
{
	if (lastNode == 0) return;		// Queue already empty

	// Point to the first element of the list
	LevelLink *l = lastNode->next;

	// As long as the first element is not also the queue, delete it
	while (l != lastNode) {
		LevelLink *ll = l;
		l = l->next;
		delete ll;
	}

	// Delete the last node in the queue
	delete lastNode;

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
}

