static const char file_id[] = "CalendarQueue.cc";
/**************************************************************************
Version identification:
@(#)CalendarQueue.cc	1.18	11/25/92

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

#ifdef __GNUG__
#pragma implementation
#endif

#include "CalendarQueue.h"

// Set the properties of the LevelLink class.
LevelLink* LevelLink :: setLink(Pointer a, double v, double fv, Star* d,
			  LevelLink* n,
			  LevelLink* b) 
{
	e = a;
	level = v;
	fineLevel = fv;
	dest = d;
	next = n;
	before = b;
	return this;
}

// get a free LevelLink if any. If none, create a new LevelLink.
// Once created, it's never destroyed.
LevelLink* CalendarQueue :: getFreeLink() 
{
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
void CalendarQueue :: putFreeLink(LevelLink* p) 
{
	p->next = freeLinkHead;
	freeLinkHead = p;
	numFreeLinks++;
}




// clear all free Links
void CalendarQueue :: clearFreeList() 
{
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








/* 
 * This initializes a bucket array within the array CalendarQ. Bucket
 * width is set equal to "startInterval". cq_bucket[0] is made equal to
 * CalendarQ[qbase]; and the number of buckets is "nbuck". StartTime is the
 * time at which dequeueing begins. All external variables except 
 * "resizeEnabled are initialized.
 */

void CalendarQueue :: LocalInit(int qbase, int nbuck, double startInterval, 
double lastTime)
{
    int i;
    long int n;

/* set position and size of new queue */
    
    cq_firstSub = qbase;
    cq_bucket = CalendarQ + qbase;
    cq_interval = startInterval;
    cq_bucketNum = nbuck;
    
/* initialize as empty */
    
    cq_eventNum = 0;
    for (i=0; i< cq_bucketNum; i++) cq_bucket[i] = NULL;
    
/* set up initial position in queue */

    cq_lastTime = lastTime;
    n = lastTime/cq_interval;
    cq_lastBucket = n % cq_bucketNum;
    cq_bucketTop = (n+1.5)*cq_interval;

/* set up queue size change thresholds */
    
    cq_bottomThreshold = cq_bucketNum/2 - 2;
    cq_topThreshold = 2*cq_bucketNum;
}



    


// lowest level first, lowest fineLevel first.

// FIXME
LevelLink* CalendarQueue :: levelput(Pointer a, double v, double fv, Star* dest)
{
    numberNodes++;
    LevelLink* newLink = getFreeLink();

    newLink->setLink(a, v, fv, dest, 0, 0);
    int i = v / cq_interval;	// find virtual bucket
    i = i % cq_bucketNum;	// find actual bucket
    InsertEventInBucket(&cq_bucket[i], newLink);
    if (cq_eventNum > cq_topThreshold && cq_bucketNum < HALF_MAX_DAYS)
	    Resize(2 * cq_bucketNum);
}



void CalendarQueue :: InsertEventInBucket(LevelLink **bucket, LevelLink *link)
{
    register LevelLink *current, *least_link = NULL;
    int i, save_resizeEnabled, virtualBucket;
    double year;

/* 
 * This is not in the paper, but you have to check if the new event
 * actually has the smallest timestamp, if so, "cq_lastTime" should
 * be reset
 */     
    if (cq_eventNum) {
	save_resizeEnabled = cq_resizeEnabled;
	cq_resizeEnabled = 0;
	least_link = NextEvent();
	i = least_link->level/cq_interval;
	i = i% cq_bucketNum;
	if (cq_bucket[i]) {
	    least_link->next = cq_bucket[i];
	    least_link->before = NULL;
	    cq_bucket[i]->before = least_link;
	} else 
	    least_link->next = least_link->before = NULL;
	cq_bucket[i]= least_link;
	cq_resizeEnabled = save_resizeEnabled;
	cq_eventNum++;
    }
    if (least_link && link->level < least_link->level) {
	cq_lastTime = link->level;
	virtualBucket = cq_lastTime/cq_interval;
	cq_lastBucket = virtualBucket%cq_bucketNum;
	cq_bucketTop = (virtualBucket+1.5)*cq_interval;
    }
    if (*bucket) {
	current = *bucket;
	// Now we are going to look for the first element
	// such that ((current->level > link->level) or
	//            (level == level) and (finer_level > link->finer_level) or
	//            (l == l) and (fl == fl) and (dest >= dest))
	while (current->level < link->level) 
	    if (current->next)
		current = current->next;
	    else {
		current->next = link;
		link->before = current;
		link->next = NULL;
		cq_eventNum++;
		return;
	    }
	assert(current->level >= link->level);
	// Moreover it is the first such element
	while ((current->level == link->level) && 
	        (current->fineLevel < link->fineLevel))
	    if (current->next)
		current = current->next;
	    else {
		current->next = link;
		link->before = current;
		link->next = NULL;
		cq_eventNum++;
		return;
	    }
	assert((current->level > link->level) || ((current->level == link->level) && (current->fineLevel >= link->fineLevel)));
	// Moreover it is the first such element
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
		return;
	    }
	assert((current->level > link->level) || ((current->level == link->level) && (current->fineLevel > link->fineLevel)) || ((current->level == link->level) && (current->fineLevel == link->fineLevel) && (current->dest >= link->dest)));
	// Moreover it is the first such element
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
    } else { /* empty bucket */
	link->before = link->next = NULL;
	*bucket = link;
    }
    cq_eventNum++;
    return;
    
}
    
	
LevelLink* CalendarQueue :: NextEvent()
{
    register LevelLink **reg_cq_bucket = cq_bucket;
    register int i, virtualBucket;
    int year;
    LevelLink *result;
    
    if (cq_eventNum == 0) return(NULL);
  again:
    for (i = cq_lastBucket;;) {       /* search buckets */
	if (reg_cq_bucket[i] && reg_cq_bucket[i]->level < cq_bucketTop) {
	    result = reg_cq_bucket[i];
	    cq_lastTime = result->level;	
	    if (reg_cq_bucket[i] = reg_cq_bucket[i]->next) 
		reg_cq_bucket[i]->before = NULL;
	    cq_lastBucket = i;
	    if (--cq_eventNum < cq_bottomThreshold)
		Resize(cq_bucketNum/2);
	    return(result);
	} else {
	    if ( ++i == cq_bucketNum) 
		i = 0;
	    cq_bucketTop += cq_interval;
	    if (i == cq_lastBucket)
		break; /* go to direct search */
	}
    }
    
/* 
 * find lowest priority by examining first event of each bucket 
 * set cq_lastBucket, cq_lastTime, cq_bucketTop                 
 */
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
    year = cq_lastTime/(cq_interval*cq_bucketNum);
    cq_bucketTop = year*cq_interval*cq_bucketNum + 
	(cq_lastBucket+1.5)*cq_interval;
    goto again;
}



//  This copies the queue onto a calendar with newsize buckets. The
//  new bucket array is now the opposite end of the array 
void CalendarQueue :: Resize(int newSize)
{
    double newInterval, NewInterval();
    int i;
    int oldBucketNum;
    struct Event *currentEvent, **oldBucket;

    if (!cq_resizeEnabled) return;

    if (newSize < 2 || newSize > QUEUE_SIZE)
	return;

    newInterval = NewInterval();

/* save location and size of old calendar for use when copying calendar */

    oldBucket = cq_bucket;
    oldBucketNum = cq_bucketNum;
    

/* initialize new calendar */

    if (cq_firstSub) 
	LocalInit(0, newSize, newInterval, cq_lastTime);
    else
	LocalInit(QUEUE_SIZE - newSize, newSize, newInterval, cq_lastTime);


    for (i = oldBucketNum - 1; i>=0; i--) {
	currentEvent = oldBucket[i];
	while (currentEvent) {
	    struct Event *nextEvent = currentEvent->next;
	    int virtualBucket;
	    virtualBucket = currentEvent->level/cq_interval;
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
    LevelLink *sampledEvent[25];
    double save_lastTime, save_bucketTop;
    int save_lastBucket;
    

/* decide how many queue elements to sample */

    if (cq_eventNum < 2) 
	return(1.0);
    if (cq_eventNum <= 5)
	sampleNum = cq_eventNum;
    else
	sampleNum = 5 + cq_eventNum/10;
    if (sampleNum > 25)
	sampleNum = 25;

/* save important dynamic values */    

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

/* insert sampled events back */

    for (i=0; i<sampleNum; i++) {
	int temp;
	temp = sampledEvent[i]->level/cq_interval; /* find virtual bucket */
	temp = temp % cq_bucketNum;      /* find actual bucket  */
	InsertEventInBucket(&cq_bucket[temp], sampledEvent[i]);
    }	

/* save important dynamic values */    

    cq_lastTime = save_lastTime;
    cq_lastBucket = save_lastBucket;
    cq_bucketTop = save_bucketTop;
    cq_resizeEnabled = 1;

/*
 * We should impose  a lower limit on the interval size. The problem can 
 * arise when the sampled events have very small intervals, while there 
 * are also events that will happen far in the future. The 
 * "NextEvent" routine would loop infinitely.
 */
    if (validSeparationNum && totalSeparation) {
	resultInterval = (double)(3*totalSeparation/validSeparationNum);
	if (resultInterval < MINI_CQ_INTERVAL) {
	    printf("interval:%f, eventNumber:%d\n", resultInterval, cq_eventNum);
	    return((double)MINI_CQ_INTERVAL);
	} else
	    return((double)resultInterval);
    } else 
	return((double)cq_interval);
}
    

void CalendarQueue :: pushBack(LevelLink* a) {
  
    numberNodes++;

    int i = a->level / cq_interval;	// find virtual bucket
    i = i % cq_bucketNum;	// find actual bucket
    InsertEventInBucket(&cq_bucket[i], newLink);
    if ((cq_resizeEnabled) &&
	(cq_eventNum > cq_topThreshold && cq_bucketNum < HALF_MAX_DAYS))
	    Resize(2 * cq_bucketNum);
}

int CalendarQueue :: fetchEvent(InDEPort* p, double timeVal)
{
	LevelLink *store = NULL;
	cq_resizeEnabled = 0;
	while (true)
	{
		LevelLink *h = NextEvent();
		if ((h == NULL) || (h->level > timeVal)) {
			Error :: abortRun (*p, " has no more data.");
			return FALSE;
		}
		InDEPort* tl = 0;
                if (h->fineLevel != 0) {
                        Event* ent = (Event*) h->e;
                        tl = (InDEPort*) ent->dest;

                        // if same destination star with same time stamp..
                        if (tl == p) {
                                if (tl->getFromQueue(ent->p))
					putFreeLink(h);
				else
					pushBack(h);
				for (;store != NULL;
				      temp = store,
				      store = store->next,
				      pushBack(temp));

				cq_resizeEnabled = 1;
                                return TRUE;
                        }
                }
		store->next->before = h;
		h->next = store;
		h->before = NULL;
		store = h;
	}
	Error :: abortRun (*p, " has no more data.");
	return FALSE;
}

void CalendarQueue :: initialize()
{
	if (cq_eventNum == 0) return;		// Queue already empty

	for (int i = cq_bucketNum - 1; i>=0; i--) {
		// Point to the first element of the list
		LevelLink *l = cq_bucket[i];

		// Put all Links into the free List.
		for (LevelLink *l = cq_bucket[i]; l != NULL; ) {
			LevelLink *ll = l;
			l = l->next;
			putFreeLink(ll);
		}

		// Delete the last node in the queue
		putFreeLink(l);

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
	
