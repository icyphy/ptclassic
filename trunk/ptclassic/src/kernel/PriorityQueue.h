/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 5/30/90
 Revision : add a list of free links to avoid memory (de)allocation at
	    each push/pop. And, add a fineLevel to LevelLinks to resolve
	    the conflicts among contents with same level. fineLevel is 
	    optional one. (11/6/90, S. Ha)

A priority queue structure.

**************************************************************************/
#ifndef _PriorityQueue_h
#define _PriorityQueue_h 1

#ifdef __GNUG__
#pragma once
#pragma interface
#endif

#include "DataStruct.h"

/**************************************************************************

 Modifier : Soonhoi Ha

Modification - To implement a priority-queue, I add another basic
container which includes *before* pointer as well as *next*, and *level*
field for its priority.

**************************************************************************/

//
// The basic container for priority queue...
//
	//////////////////////////////////////
	// class LevelLink
	//////////////////////////////////////

class LevelLink 
{
	friend class PriorityQueue;

	LevelLink* next;
	LevelLink* before;

	LevelLink* setLink(Pointer a, float v, float fv, LevelLink* n, LevelLink* b);
public:
	Pointer e;
	float	level;		// currently, level is "float" type.
	float fineLevel;	// If levels are equal, we may need
				// finerLevel which is optional.
	LevelLink() {}
};

	//////////////////////////////////////
	// class PriorityQueue
	//////////////////////////////////////

class PriorityQueue
{
	int numberNodes;
	LevelLink* freeLinkHead;
	int  	   numFreeLinks;		// mainly for debugging.

protected:

	// Store head of the queue, lastNode->next is head of queue
	LevelLink* lastNode;

	// Store last access to the list for sequential access
	LevelLink* lastReference;

	// To avoid memory (de)allocation overhead at each push/pop, we
	// store the freeLinks once created.
	LevelLink* getFreeLink();
	virtual void 	   clearFreeList();

public:
	// Add element to the queue sorted by its level from the tail.
	LevelLink* levelput(Pointer a, float v, float fv = 1.0);

	// Add element to the queue sorted by its level from the head.
	LevelLink* leveltup(Pointer a, float v, float fv = 1.0);

	// append the link to the end of the queue (ignore levels).
	void put(Pointer a, float v = 0);

	// Push back the link just gotten.
	void pushBack(LevelLink*);

	// Remove and return link from the head of the queue...
	LevelLink* get();

	// Return the next node on list, relative to the last reference
	LevelLink* next();

	// Reset the reference pointer
	void reset() {lastReference = lastNode ;}

	// Extract a link within the queue, which deviates from the
   	// definition of "queue", but necessary-and-convenient method
	// for getting all elements of same level and of same property...
	// The extracted Link is out into the free List.
	void extract(LevelLink*);

	// Return number of elements currently in queue...
	int length() {return numberNodes;}

	// clear the queue...
	// move all Links into the free List.
	void initialize();

	// put the link into the pool of free links.
	virtual void 	   putFreeLink(LevelLink*);

	// Constructor
	PriorityQueue() : freeLinkHead(0), numberNodes(0), 
			  lastNode(0), numFreeLinks(0) {}
	PriorityQueue(Pointer a, float v, float fv) ;
	~PriorityQueue();
};



inline LevelLink* PriorityQueue :: next()
{
	lastReference = lastReference->next;
	return lastReference;
}
		
	
#endif
