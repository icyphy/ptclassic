static const char file_id[] = "MutableCalendarQueue.cc";
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1997 The Regents of the University of California.
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

 Programmer: John Davis
 Date: 5/19/97
        This class is extended from CalendarQueue. We make this
        extension rather than adding the functionality directly
        to the CalendarQueue class so that the user can easily
        switch between the "regular" CalendarQueue and the
        MutableCalendarQueue.

**************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "EventQueue.h"
#include "MutableCalendarQueue.h"
#include <assert.h>
#include <limits.h>             // We use INT_MAX below


// Remove a CqLevelLink from the Calendar Queue
// in response to a dying star. 
int MutableCalendarQueue::removeEvent( CqLevelLink * cqLevelLink )
{
	MutableEvent * event;
	double level, fineLevel;
	unsigned long absEventNum;
	Star * dest;
	int virtualBucket, actualBucket;
	CqLevelLink * current;

	event = cqLevelLink->e;
	level = cqLevelLink->level;
	fineLevel = cqLevelLink->fineLevel;
	absEventNum = cqLevelLink->absEventNum;
	dest = cqLevelLink->dest;

	virtualBucket = (int) (level/cq_interval);
	actualBucket = virtualBucket%cq_bucketNum;

	if( cq_bucket[actualBucket] )
	{
	     current = cq_bucket[actualBucket];


	     // Sort on time stamp (level)
	     while( current->level < level )
	     {
		  if( current->next )
		  {
		       current = current->next;
		  }
	     }
	     if( !current->next && current->level != level )
	     {
		  // Reached end of the road without finding event
		  Error::abortRun( "Can't Find Event Within Calendar Queue!" );
		  return 0;
	     }


	     // Sort on fineLevel
	     while( current->fineLevel < fineLevel )
	     {
		  if( current->next )
		  {
		       current = current->next;
		  }
	     }
	     if( !current->next && current->fineLevel != fineLevel )
	     {
		  // Reached end of the road without finding event
		  Error::abortRun( "Can't Find Event Within Calendar Queue!" );
		  return 0;
	     }


	     // Sort on destination
	     while( current->dest < dest )
	     {
		  if( current->next )
		  {
		       current = current->next;
		  }
	     }
	     if( !current->next && current->dest != dest )
	     {
		  // Reached end of the road without finding event
		  Error::abortRun( "Can't Find Event Within Calendar Queue!" );
		  return 0;
	     }


	     // Sort on event number
	     while( current->absEventNum < absEventNum )
	     {
		  if( current->next )
		  {
		       current = current->next;
		  }
	     }
	     if( !current->next && current->dest != dest )
	     {
		  // Reached end of the road without finding event
		  Error::abortRun( "Can't Find Event Within Calendar Queue!" );
		  return 0;
	     }


	     // We've successfully made it!!
	     cq_eventNum--;
	     cq_absEventCounter--;

	     // Take CqLevelLink out of calendar queue
	     current->before->next = current->next;
	     current->before = 0;
	     current->next = 0;

	     // Put CqLevelLink in the free list
	     putFreeLink( current );

	}

	if(  (cq_resizeEnabled) && 
	     (cq_eventNum > cq_topThreshold && cq_bucketNum < HALF_MAX_DAYS)  )
	{
	     Resize( cq_bucketNum/2 );
	}


	return 1;

}


CqLevelLink* MutableCalendarQueue :: NextEvent()
{
    register CqLevelLink **reg_cq_bucket = cq_bucket;
    register int i;
    int year;
    CqLevelLink *result;
    Link * destinedLink;
    Block * blockPtr = 0;

   
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
            if ( (reg_cq_bucket[i] = reg_cq_bucket[i]->next) )
                reg_cq_bucket[i]->before = NULL;
            cq_lastBucket = i;
            cq_eventNum--;
            if ((cq_resizeEnabled) && (cq_eventNum < cq_bottomThreshold))
                Resize(cq_bucketNum/2);
            if (cq_debug) printf("REMOVE: ev # %ld, time %f, fine level %f\n",
                        result->absEventNum, result->level, result->fineLevel);



	    // ************************************************ //
	    // Remove the corresponding element within the 
	    // DestinedEventList.
	    // ************************************************ //
	    destinedLink = result->destinedLink;
	    result->destinedLink = 0;
	    blockPtr = result->e->dest;
	    if( blockPtr->destinedEventList->remove( destinedLink ) )
	    {
		 return result;
	    }
	    else
	    {
		 Error::abortRun
		 ("Can't Remove Link on DestinedEventList Within NextEvent()");
	    }
	    // ************************************************ //



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




