/**************************************************************************
Version identification:
@(#)CalendarQueue.h	1.15	11/25/92

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


**************************************************************************/
#ifndef _CalendarQueue_h
#define _CalendarQueue_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "DataStruct.h"


#define MAX_BUCKET     1024*4
#define QUEUE_SIZE     (MAX_BUCKET*3)/2
#define HALF_MAX_DAYS  1024*2  
#define MINI_CQ_INTERVAL  0.2




//
// The basic container for priority queue...
//
	//////////////////////////////////////
	// class LevelLink
	//////////////////////////////////////

class LevelLink 
{
	friend class CalendarQueue;
public:
	Pointer e;
	double	level;		// currently, level is "double" type.
	double fineLevel;	// If levels are equal, we may need
				// finerLevel which is optional.
	LevelLink() {}

private:
	LevelLink* next;
	LevelLink* before;

	// v sets the level, and fv sets the fineLevel of the entry.
	// Numerically smaller number represents  the higher priority.
	LevelLink* setLink(Pointer a, double v, double fv, LevelLink* n, LevelLink* b);
};

	//////////////////////////////////////
	// class CalendarQueue
	//////////////////////////////////////

class CalendarQueue
{
public:
	// Add element to the tail of the queue and sort it by its level (v)
	// first and its fineLevel (fv) second.
	// Numerically smaller number represents the higher priority.
	// (i.e., highest level is at the tail)
	LevelLink* levelput(Pointer a, double v, double fv = 1.0);

	// Push back the link just gotten.
	void pushBack(LevelLink*);

	// Remove and return link from the head of the queue...
	Pointer getFirstElem();

	// Remove and return link from the head of the queue...
	// WARNING -- Must call putFreeLink() after finishing with it.
	// Use getFirstElem() to get and free in one step.
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

// FIXME
	// Constructor
	CalendarQueue() : freeLinkHead(0), numberNodes(0), numFreeLinks(0), 
			  cq_resizeEnabled(1) 
        { LocalInit(0, 2, 1.0, 0.0); }
	virtual ~CalendarQueue();

protected:
	LevelLink **cq_bucket;

	int cq_resizeEnabled;  
	int cq_lastBucket;      /* bucket number last event was dequeued */
	double cq_bucketTop;    /* priority at the top of that bucket    */
	double cq_lastTime;     /* priority of last dequeued event       */

	int cq_bottomThreshold, cq_topThreshold;  
	int cq_bucketNum;       /* number of buckets                     */
	int cq_eventNum;               /* number of events in calendar queue    */
	double cq_interval;        /* size of intervals of each bucket      */
	int cq_firstSub;
	LevelLink* CalendarQ[QUEUE_SIZE];

	// Store last access to the list for sequential access
	LevelLink* lastReference;

	// To avoid memory (de)allocation overhead at each push/pop, we
	// store the freeLinks once created.
	LevelLink* getFreeLink();
	virtual void 	   clearFreeList();

	void LocalInit(int qbase, int nbuck, double startInterval, double lastTime);
	void InsertEventInBucket(LevelLink **bucket, LevelLink *event);
	LevelLink* NextEvent();
	void Resize(int newSize);
	double NewInterval();

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
inline LevelLink* CalendarQueue :: next()
{
	lastReference = lastReference->next;
	return lastReference;
}
		
	
#endif
