static const char file_id[] = "CalendarQueue.cc";
/**************************************************************************
Version identification:
 $Id$

WANRNING experimental version
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

**************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "EventQueue.h"
#include "CalendarQueue.h"
#include <assert.h>

// Set the properties of the CqLevelLink class.
CqLevelLink* CqLevelLink :: setLink(Pointer a, double v, double fv,
			  Star* d, unsigned long abs,
			  CqLevelLink* n, CqLevelLink* b) 
{
	e = a;
	level = v;
	fineLevel = fv;
	dest = d;
	absEventNum = abs;
	next = n;
	before = b;
	return this;
}

// get a free CqLevelLink if any. If none, create a new CqLevelLink.
// Once created, it's never destroyed.
CqLevelLink* CalendarQueue :: getFreeLink() 
{
	CqLevelLink* temp;
	if (freeLinkHead) {
		temp = freeLinkHead;
		freeLinkHead = temp->next;
		numFreeLinks--;
	} else {
		LOG_NEW; temp = new CqLevelLink;
	}
	return temp;
}

// put an unsed CqLevelLink into the free List for later use.
void CalendarQueue :: putFreeLink(CqLevelLink* p) 
{
	p->next = freeLinkHead;
	freeLinkHead = p;
	numFreeLinks++;
}




// clear all free Links
void CalendarQueue :: clearFreeList() 
{
	if (!freeLinkHead) return;
	CqLevelLink* temp;
	while (freeLinkHead->next) {
		temp = freeLinkHead;
		freeLinkHead = freeLinkHead->next;
		LOG_DEL; delete temp;
	}
	LOG_DEL; delete freeLinkHead;
	numFreeLinks = 0;
}


// This initializes a bucket array within the array CalendarQ. Bucket
// width is set equal to "startInterval". cq_bucket[0] is made equal to
// CalendarQ[qbase]; and the number of buckets is "nbuck". StartTime is the
// time at which dequeueing begins. All external variables except 
// "resizeEnabled are initialized.

void CalendarQueue :: LocalInit(int qbase, int nbuck, double startInterval, 
double lastTime)
{
    int i;
    long int n;

// set position and size of new queue 
    
    cq_firstSub = qbase;
    cq_bucket = CalendarQ + qbase;
    cq_interval = startInterval;
    cq_bucketNum = nbuck;
    
// initialize as empty 
    
    cq_eventNum = 0;
    for (i=0; i< cq_bucketNum; i++) cq_bucket[i] = NULL;
    
// set up initial position in queue 

    cq_lastTime = lastTime;
    n = (int) (lastTime/cq_interval);
    cq_lastBucket = (int) (n % cq_bucketNum);
    cq_bucketTop = (n+1.5)*cq_interval;

// set up queue size change thresholds 
    
    cq_bottomThreshold = cq_bucketNum/2 - 2;
    cq_topThreshold = 2*cq_bucketNum;

}

// lowest level first, lowest fineLevel first.

CqLevelLink* CalendarQueue :: levelput(Pointer a, double v, double fv, Star* dest)
{
    CqLevelLink* newLink = getFreeLink();
    // use the event counter to set the finest level of the sort
    newLink->setLink(a, v, fv, dest, cq_absEventCounter, 0, 0);
    int i = (int) (v / cq_interval);	// find virtual bucket
    i = i % cq_bucketNum;	// find actual bucket
    InsertEventInBucket(&cq_bucket[i], newLink);
    if ((cq_resizeEnabled) &&
	(cq_eventNum > cq_topThreshold && cq_bucketNum < HALF_MAX_DAYS))
	    Resize(2 * cq_bucketNum);
    return newLink;
}

void CalendarQueue :: InsertEventInBucket(CqLevelLink **bucket, CqLevelLink *link)
{
    register CqLevelLink *current = NULL;
    int virtualBucket;

    // This is not in the paper, but you have to check if the new event actually 
    // has the smallest timestamp, if so, "cq_lastTime" should be reset.
    // I am overriding Hui's patch with mine which checks for the
    // condition on which the bucket needs to be set based on the
    // present bucket position and the new event, rather than the
    // youngest event previously in the queue. This is faster because
    // we do not need to fetch the event. I am inserting an assert
    // in NextEvent which should check for the correctness of this
    // way of doing things. - Anindo Banerjea


    if ( link->level < cq_lastTime ) {
        cq_lastTime = link->level;
        virtualBucket = (int)(cq_lastTime/cq_interval);
        cq_lastBucket = virtualBucket%cq_bucketNum;
        cq_bucketTop = (virtualBucket+1.5)*cq_interval;
    }
    if (*bucket) {
	current = *bucket;
        if (cq_debug) printf("INSERT1: last time %f, ev # %ld, time %f, fine level %f\n", 
	cq_lastTime, link->absEventNum, link->level, link->fineLevel);
	// Now we are going to look for the first element
	// such that ((current->level > link->level) or
	//            (level == level) and (finer_level > link->finer_level) or
	//            (l==l) and (fl==fl) and (current->dest > link->dest))
	// 	      (l==l) and (fl==fl) and (dest == dest) 
	// 	             and (current->absEventNum > link->absEventNum)


	// Sort on time stamp
	while (current->level < link->level) 
	    if (current->next)
		current = current->next;
	    else {
		current->next = link;
		link->before = current;
		link->next = NULL;
		cq_eventNum++;
		cq_absEventCounter++;
		return;
	    }
	
	// Sort on fine level (topological sort)
	while ((current->level == link->level) && 
	        (current->fineLevel < link->fineLevel))
	    if (current->next)
		current = current->next;
	    else {
		current->next = link;
		link->before = current;
		link->next = NULL;
		cq_eventNum++;
		cq_absEventCounter++;
		return;
	    }

	// Sort on destination
	while  ((current->level == link->level) &&
		(current->fineLevel == link->fineLevel) &&
		(current->dest < link->dest))
	    if (current->next)
		current = current->next;
	    else {
		current->next = link;
		link->before = current;
		link->next = NULL;
		cq_eventNum++;
		cq_absEventCounter++;
		return;
	    }

	// Sort on event number so that events with all of the above the
	// same will be sorted by the order that they were inserted
	while  ((current->level == link->level) &&
		(current->fineLevel == link->fineLevel) &&
		(current->dest == link->dest) &&
		(current->absEventNum < link->absEventNum))
	    if (current->next)
		current = current->next;
	    else {
		current->next = link;
		link->before = current;
		link->next = NULL;
		cq_eventNum++;
		cq_absEventCounter++;
		return;
	    }


	// Done sorting, put it in the bucket
	if (current->before) {
	    link->before = current->before;
	    link->next = current;
	    current->before->next = link;
	    current->before = link;
	}  else {
	    link->before = NULL;
	    link->next = current;
	    current->before = link;
	    *bucket = link;
	}
    } else { // empty bucket 
        if (cq_debug) printf("INSERT2: at time %f, ev # %ld, time %f, fine level %f\n", 
	cq_lastTime, link->absEventNum, link->level, link->fineLevel);
	link->before = link->next = NULL;
	*bucket = link;
    }
    cq_eventNum++;
    cq_absEventCounter++;
    return;
    
}
    
	
CqLevelLink* CalendarQueue :: NextEvent()
{
    register CqLevelLink **reg_cq_bucket = cq_bucket;
    register int i;
    int year;
    CqLevelLink *result;

    
    if (cq_eventNum == 0) return(NULL);
  again:
    for (i = cq_lastBucket;;) {       /* search buckets */
	if (reg_cq_bucket[i] && reg_cq_bucket[i]->level < cq_bucketTop) {
	    result = reg_cq_bucket[i];
	    // This checks for my tweak which sets the lastBucket stuff
	    // in InsertEventInBucket. If this fails this means that the
	    // lastBucket stuff was not correctly set because we have
	    // found an event from the past still lying around.
            // (cq_bucketTop - 1.5*cq_interval ) 
            // is the bottom of the lastBucket in time units.
	    // We use 1.6 here to account for rounding off errors which
	    // might cause a event to look bad if it was just on the border.
	    assert(result->level >= ( cq_bucketTop- 1.6*cq_interval ));
	    cq_lastTime = result->level;	
	    if (reg_cq_bucket[i] = reg_cq_bucket[i]->next) 
		reg_cq_bucket[i]->before = NULL;
	    cq_lastBucket = i;
	    cq_eventNum--;
	    if ((cq_resizeEnabled) && (cq_eventNum < cq_bottomThreshold))
		Resize(cq_bucketNum/2);
	    if (cq_debug) printf("REMOVE: ev # %ld, time %f, fine level %f\n",
	 		result->absEventNum, result->level, result->fineLevel);
	    return(result);
	} else {
	    if ( ++i == cq_bucketNum) 
		i = 0;
	    cq_bucketTop += cq_interval;
	    if (i == cq_lastBucket)
		break; /* go to direct search */
	}
    }
    
// find lowest priority by examining first event of each bucket 
// set cq_lastBucket, cq_lastTime, cq_bucketTop                 
    result = NULL;
    for (i=0; i<cq_bucketNum; i++) 
	if (reg_cq_bucket[i]) {
	    result = reg_cq_bucket[i];
	    cq_lastBucket = i;
	    break;
	}
    if (result == NULL) {
	fprintf(stderr,"NextEvent\n");
	exit(1);
    }
    for (i=cq_lastBucket+1; i<cq_bucketNum; i++) {
	if (reg_cq_bucket[i] && reg_cq_bucket[i]->level < result->level)  {
	    result = reg_cq_bucket[i];
	    cq_lastBucket = i;
	}
    }
	    
    cq_lastTime = result->level;
    year = (int)(cq_lastTime/(cq_interval*cq_bucketNum));
    cq_bucketTop = year*cq_interval*cq_bucketNum + 
	(cq_lastBucket+1.5)*cq_interval;
    goto again;
}



//  This copies the queue onto a calendar with newsize buckets. The
//  new bucket array is now the opposite end of the array 
void CalendarQueue :: Resize(int newSize)
{
    double newInterval;
    int i;
    int oldBucketNum;
    CqLevelLink *currentEvent, **oldBucket;

    if (!cq_resizeEnabled) return;

    if (newSize < 2 || newSize > QUEUE_SIZE)
	return;

    newInterval = NewInterval();

// save location and size of old calendar for use when copying calendar 

    oldBucket = cq_bucket;
    oldBucketNum = cq_bucketNum;
    

// initialize new calendar 

    if (cq_firstSub) 
	LocalInit(0, newSize, newInterval, cq_lastTime);
    else
	LocalInit(QUEUE_SIZE - newSize, newSize, newInterval, cq_lastTime);


    for (i = oldBucketNum - 1; i>=0; i--) {
	currentEvent = oldBucket[i];
	while (currentEvent) {
	    CqLevelLink *nextEvent = currentEvent->next;
	    int virtualBucket;
	    virtualBucket = (int)(currentEvent->level/cq_interval);
	    virtualBucket = virtualBucket% cq_bucketNum; 
	    InsertEventInBucket(&cq_bucket[virtualBucket], currentEvent);
	    currentEvent = nextEvent;
	}
    }
	    
}


double CalendarQueue :: NewInterval()
{
    int sampleNum;
    int i;
    int validSeparationNum;
    double resultInterval;
    double eventSeparation[25], totalSeparation, twiceAverageSeparation;
    CqLevelLink *sampledEvent[25];
    double save_lastTime, save_bucketTop;
    int save_lastBucket;
    

// decide how many queue elements to sample 

    if (cq_eventNum < 2) 
	return(1.0);
    if (cq_eventNum <= 5)
	sampleNum = cq_eventNum;
    else
	sampleNum = 5 + cq_eventNum/10;
    if (sampleNum > 25)
	sampleNum = 25;

// save important dynamic values 

    save_lastTime = cq_lastTime;
    save_lastBucket = cq_lastBucket;
    save_bucketTop = cq_bucketTop;

    cq_resizeEnabled = 0;
    
    sampledEvent[0] = NextEvent();
    eventSeparation[0] = sampledEvent[0]->level;

    totalSeparation = 0;
    for (i=1; i<sampleNum; i++) {
	sampledEvent[i] = NextEvent();
	eventSeparation[i] = sampledEvent[i]->level;
	eventSeparation[i-1] = 
	    eventSeparation[i] - eventSeparation[i-1];
	totalSeparation += eventSeparation[i-1];
    }
    
    twiceAverageSeparation = 2*totalSeparation/(sampleNum - 1);
    
    totalSeparation = 0;
    validSeparationNum = 0;
    for (i=0; i<sampleNum-1; i++) 
	if (eventSeparation[i] < twiceAverageSeparation) {
	    totalSeparation += eventSeparation[i];
	    validSeparationNum++;
	}

// insert sampled events back 

    for (i=0; i<sampleNum; i++) {
	int temp;
	temp = (int)(sampledEvent[i]->level/cq_interval); // find virtual bucket
	temp = temp % cq_bucketNum;      // find actual bucket  
	InsertEventInBucket(&cq_bucket[temp], sampledEvent[i]);
    }	

// save important dynamic values 

    cq_lastTime = save_lastTime;
    cq_lastBucket = save_lastBucket;
    cq_bucketTop = save_bucketTop;
    cq_resizeEnabled = 1;

// We should impose  a lower limit on the interval size. The problem can 
// arise when the sampled events have very small intervals, while there 
// are also events that will happen far in the future. The 
// "NextEvent" routine would loop infinitely.
    if (validSeparationNum && totalSeparation) {
	resultInterval = (double)(3*totalSeparation/validSeparationNum);
	if (resultInterval < MINI_CQ_INTERVAL) {
	    return((double)MINI_CQ_INTERVAL);
	} else
	    return((double)resultInterval);
    } else 
	return((double)cq_interval);
}
    

void CalendarQueue :: pushBack(CqLevelLink* a) {
  

    int i = (int)(a->level / cq_interval);	// find virtual bucket
    i = i % cq_bucketNum;	// find actual bucket
    InsertEventInBucket(&cq_bucket[i], a);
    if ((cq_resizeEnabled) &&
	(cq_eventNum > cq_topThreshold && cq_bucketNum < HALF_MAX_DAYS))
	    Resize(2 * cq_bucketNum);
}

void CalendarQueue :: initialize()
{
	if (cq_eventNum == 0) {
	    return;		// Queue already empty
	}
	for (int i = cq_bucketNum - 1; i>=0; i--) {

		// Put all Links into the free List.
		for (CqLevelLink *l = cq_bucket[i]; l != NULL;) {
			CqLevelLink *ll = l;
			l = l->next;
			putFreeLink(ll);
		}

		// and mark the queue empty
		cq_bucket[i] = 0;
	}

	cq_eventNum = 0;
}

// Destructor -- delete all Links 
CalendarQueue :: ~CalendarQueue () {
	initialize();
	clearFreeList();
}
	
