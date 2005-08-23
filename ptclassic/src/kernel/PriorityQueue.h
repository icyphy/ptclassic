/**************************************************************************
Version identification:
@(#)PriorityQueue.h	1.19	3/2/95

Copyright (c) 1990-1995 The Regents of the University of California.
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
#pragma interface
#endif

#include "type.h"
class Particle;
class PortHole;

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
public:
	Pointer e;
	double	level;		// currently, level is "double" type.
	double fineLevel;	// If levels are equal, we may need
				// finerLevel which is optional.
	LevelLink() {}

protected:
	LevelLink* next;
	LevelLink* before;

	// v sets the level, and fv sets the fineLevel of the entry.
	// Numerically smaller number represents  the higher priority.
	LevelLink* setLink(Pointer a, double v, double fv, LevelLink* n, LevelLink* b);
};

	//////////////////////////////////////
	// class PriorityQueue
	//////////////////////////////////////


class BasePrioQueue
{
public:
    virtual void pushHead(Particle* p, PortHole* ph, double v, double fv);
    virtual void pushTail(Particle* p, PortHole* ph, double v, double fv);
    virtual LevelLink* levelput(Pointer a, double v, double fv);
};

class PriorityQueue : public BasePrioQueue
{
public:
	// Add element to the tail of the queue and sort it by its level (v)
	// first and its fineLevel (fv) second.
	// Numerically smaller number represents the higher priority.
	// (i.e., highest level is at the tail)
	LevelLink* levelput(Pointer a, double v, double fv = 1.0);

	// Add element to the head of the queue and sort it by its level (v)
	// first and its fineLevel (fv) second.
	// Numerically smaller number represents the higher priority.
	// (i.e., highest level is at the head)
	LevelLink* leveltup(Pointer a, double v, double fv = 1.0);

	// append the link to the end of the queue (ignore levels).
	// Value v is necessary to fill a link (the fineLevel of the link is
	// set to 1.0 -- default value).
	void put(Pointer a, double v = 0);

	// Push back the link just gotten.
	void pushBack(LevelLink*);

	// Remove and return link from the head of the queue...
	Pointer getFirstElem();

	// Remove and return link from the head of the queue...
	// WARNING -- Must call putFreeLink() after finishing with it.
	// Use getFirstElem() to get and free in one step.
	LevelLink* get();

	// Remove and return link from the tail of the queue...
	Pointer getLastElem();

	// Remove and return link from the tail of the queue...
	// WARNING -- Must call putFreeLink() after finishing with it.
	// Use getLastElem() to get and free in one step.
	LevelLink* teg();

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
	PriorityQueue() : lastNode(0), numberNodes(0),
	                  freeLinkHead(0), numFreeLinks(0) {}
	PriorityQueue(Pointer a, double v, double fv) ;
	virtual ~PriorityQueue();

protected:
	// Store head of the queue, lastNode->next is head of queue
	LevelLink* lastNode;

	// Store last access to the list for sequential access
	LevelLink* lastReference;

	// To avoid memory (de)allocation overhead at each push/pop, we
	// store the freeLinks once created.
	LevelLink* getFreeLink();
	virtual void 	   clearFreeList();

private:
	int numberNodes;
	// These are for memory management scheme to minimize the dynamic
	// memory (de)allocation. FreeLinks are managed in linked-list
	// structure.
	LevelLink* freeLinkHead;
	int  	   numFreeLinks;		// mainly for debugging.
};



// If it ends up with the last link of the queue, it rounds up the head of
// the queue.
inline LevelLink* PriorityQueue :: next()
{
	lastReference = lastReference->next;
	return lastReference;
}
		
	
#endif
