/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 5/30/90

A priority queue structure.

**************************************************************************/
#ifndef _PriorityQueue_h
#define _PriorityQueue_h 1

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
public:
	Pointer e;
	float	level;		// currently, level is "float" type.

	LevelLink(Pointer a, float v, LevelLink* np, LevelLink* bp)
		{e = a; level = v; next = np; before = bp ;}
};

	//////////////////////////////////////
	// class PriorityQueue
	//////////////////////////////////////

class PriorityQueue
{
	int numberNodes;

	// Store head of the queue, lastNode->next is head of queue
	LevelLink* lastNode;

	// Store last access to the list for sequential access
	LevelLink* lastReference;

public:
	// Add element to the queue sorted by its level...
	void levelput(Pointer, float);

	// Remove and return link from the head of the queue...
	LevelLink* get();

	// Return the next node on list, relative to the last reference
	LevelLink* next();

	// Reset the reference pointer
	void reset() {lastReference = lastNode ;}

	// Extract a link within the queue, which deviates from the
   	// definition of "queue", but necessary-and-convenient method
	// for getting all elements of same level and of same property...
	void extract(LevelLink*);

	// Return number of elements currently in queue...
	int length() {return numberNodes;}

	// clear the queue...
	void initialize();

	// Constructor
	PriorityQueue() {numberNodes = 0; lastNode = 0;}
	PriorityQueue(Pointer, float) ;
};



inline LevelLink* PriorityQueue :: next()
{
	lastReference = lastReference->next;
	return lastReference;
}
		
	
#endif
