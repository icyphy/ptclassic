/**************************************************************************
Version identification:
$Id$ $Revision$


Copyright (c) 1990-1994 The Regents of the University of California.
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

#include <math.h>
#ifdef sgi
extern const double __infinity;
#define HUGE_VAL __infinity	
#endif

#ifndef HUGE_VAL
// Some users report that HUGE_VAL is not defined for them.
// (sun4, gcc-2.4.5, gcc-2.5.8)  There could be nasty problems here.
// linux had problems with the statistics demo and a SIGFPE.
#define HUGE_VAL MAXDOUBLE
#endif

#include "DataStruct.h"
#include "DEStar.h"
#include "PriorityQueue.h"


#define MAX_BUCKET     1024*4
#define QUEUE_SIZE     (MAX_BUCKET*3)/2
#define HALF_MAX_DAYS  1024*2  
#define MINI_CQ_INTERVAL  0.2




//
// The basic container for priority queue...
//
	//////////////////////////////////////
	// class CqLevelLink
	//////////////////////////////////////

class CqLevelLink : public LevelLink
{
	friend class CalendarQueue;
public:
	Star* dest;             // All the destinations are also stored
				// contiguously to allow sequential
				// popping off of same Star event;
				// of an event. Finally, if events have
	unsigned long absEventNum;   // all of the above the same, we still
				     // need to have events sorted by the order 
				     // that they arrived
	CqLevelLink() {}

	CqLevelLink* next;
	CqLevelLink* before;

	// v sets the level, and fv sets the fineLevel of the entry.
	// Numerically smaller number represents  the higher priority.
	CqLevelLink* setLink(Pointer a, double v, double fv,
			   Star* d, unsigned long abs, 
			   CqLevelLink* n, CqLevelLink* b);
};

	//////////////////////////////////////
	// class CalendarQueue
	//////////////////////////////////////

class CalendarQueue : public BasePrioQueue
{
public:
	// Add element to the tail of the queue and sort it by its level (v)
	// first and its fineLevel (fv) second.
	// Numerically smaller number represents the higher priority.
	// (i.e., highest level is at the tail)
	CqLevelLink* levelput(Pointer a, double v, double fv, Star* dest);

	// Push back the link just gotten.
	void pushBack(CqLevelLink*);

	// Remove and return link from the head of the queue...
	Pointer getFirstElem() {
		CqLevelLink *f = get();
		putFreeLink(f);
		return f->e;
	}

	// Remove and return link from the head of the queue...
	// WARNING -- Must call putFreeLink() after finishing with it.
	// Use getFirstElem() to get and free in one step.
	CqLevelLink* get() {
	     CqLevelLink *h = NextEvent(); 
	     return h;
	}

	// Return number of elements currently in queue...
	int length() {return cq_eventNum;}

	// clear the queue...
	// move all Links into the free List.
	void initialize();

	// put the link into the pool of free links.
	virtual void 	   putFreeLink(CqLevelLink*);

	void EnableResize() { cq_resizeEnabled = 1; }
	void DisableResize() { cq_resizeEnabled = 0; }

	// Constructor
	CalendarQueue() : cq_debug(0), cq_eventNum(0),
	  		  cq_absEventCounter(0), freeLinkHead(0),
	                  numFreeLinks(0), cq_resizeEnabled(1)
        { LocalInit(0, 2, 1.0, HUGE_VAL ); }
	virtual ~CalendarQueue();

protected:

	CqLevelLink **cq_bucket;

	int cq_debug;           // Used to turn on debug statements
	int cq_lastBucket;      // bucket number last event was dequeued 
	double cq_bucketTop;    // priority at the top of that bucket   
	double cq_lastTime;     // priority of last dequeued event     

	int cq_bottomThreshold, cq_topThreshold;  
	int cq_bucketNum;       // number of buckets                  
	int cq_eventNum;        // number of events in calendar queue 
	double cq_interval;     // size of intervals of each bucket  
	unsigned long cq_absEventCounter;  // keep a count of the total events
				// received in order to sort on
				// the order events were sent
	int cq_firstSub;
	CqLevelLink* CalendarQ[QUEUE_SIZE];

	// To avoid memory (de)allocation overhead at each push/pop, we
	// store the freeLinks once created.
	CqLevelLink* getFreeLink();
	virtual void 	   clearFreeList();

	void LocalInit(int qbase, int nbuck, double startInterval, double lastTime);
	void InsertEventInBucket(CqLevelLink **bucket, CqLevelLink *event);
	CqLevelLink* NextEvent();
	void Resize(int newSize);
	double NewInterval();

private:
	// These are for memory management scheme to minimize the dynamic
	// memory (de)allocation. FreeLinks are managed in linked-list
	// structure.
	CqLevelLink* freeLinkHead;
	int  	   numFreeLinks;		// mainly for debugging.
	int cq_resizeEnabled;  
};


#endif
