#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/* Copyright (c) 1990-1993 The Regents of the University of California.
 * All rights reserved.
 * 
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the above
 * copyright notice and the following two paragraphs appear in all copies
 * of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
 * THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE.
 * 
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
 * CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.
 * 
 */
/*LINTLIBRARY*/
/*
 * VEM Oct Window Management Routines
 * Redraw Queue Management
 *
 * David Harrison
 * University of California, Berkeley
 * 1988
 *
 * The VEM window package attempts to provide a very high level
 * interface to the display of a graphics window.  Basically,  there
 * are two operations exported to the outside world:  redisplay window
 * and redisplay virtual region.  Redisplay window (wnQWindow) is
 * used to update a window after a window specific operation like
 * zoom or pan.  Redisplay virtual region draws a rectangular area
 * of an Oct facet in all applicable windows.  This is the primary
 * means for most commands to update displays.  It is also intended
 * that this be used by the change list daemon when it becomes
 * available.
 *
 * Note that wnQWindow and wnQRedisplay do not update the display.  They
 * simply add the redisplay requests to an internal queue.  The user
 * must call wnFlush to actually carry out the queued requests.
 *
 * In VEM 5-0,  a new queue for handling selection redraw has been
 * added.  This call, wnQSelect, causes an update to a specified
 * region of a selected set.  When selection is allocated on its
 * own plane,  this can be done with significant performance improvements
 * over simply redrawing the area (as was done in VEM 4-6 and earlier).
 * Note also that wnQRedraw used to be exported but is no longer exported.
 * It has been replaced with wnQWindow which was found to fit the
 * bill better.
 */

#include "wininternal.h"	/* Internal view of windows */
#include "objsel.h"		/* Object selection         */
#include "vemUtil.h"		/* General VEM utilities    */
#include "message.h"		/* Message                  */

/* Forward declarations for lint */

static int queueRedraw();
  /* Queues standard redraw event */
static vemStatus normFlush();
  /* Flushes normal queue */
static vemStatus selFlush();
  /* Flushes selection queue */



/* Queue of redraw events */

typedef struct rd_request_defn {
    internalWin *theWin;	/* Window to redraw            */
    struct octBox extent;	/* Extent in Oct coordinates   */
} rd_request;

#define QUEUE_SIZE	128

static rd_request rd_queue[QUEUE_SIZE];
static int rd_last = 0;

/* Queue of selection events */

typedef struct sel_request_defn {
    octId theFct;		/* Buffer of redraw sets       */
    atrHandle theAttr;		/* Attribute of redraw sets    */
    struct octBox extent;	/* Extent in Oct coordinates   */
} sel_request;

static sel_request sel_queue[QUEUE_SIZE];
static int sel_last = 0;

/* Structure used for searching through windows */

typedef struct bufferExtent_defn {
    octId searchFct;		/* Facet to compare against */
    struct octBox extent;	/* Extent in buffer         */
} bufferExtent;


void _wnQRemove(theWin)
internalWin *theWin;
/*
 * Eliminates the outstanding events for `theWin' from the
 * normal redraw queue.
 */
{
    register int idx;

    for (idx = 0;  idx < rd_last;  idx ++) {
	if (rd_queue[idx].theWin == theWin) {
	    /* Remove from queue */
	    rd_queue[idx] = rd_queue[rd_last - 1];
	    rd_last--;
	}
    }
}


vemStatus wnQWindow(win)
Window win;			/* Window to redraw */
/*
 * This routine queues a request to redraw the specified VEM graphics
 * window.  These requests will be carried out on the next call to
 * wnFlush().  This allows the routine to merge requests to minimize
 * drawing time (if possible).  This routine basically calls on
 * wnQRedraw with the right parameters.
 */
{
    internalWin *theWin;
    int winstate;

    if (_wnFind(win, &theWin, &winstate, (int *) 0)) {
	if (winstate != WN_ACTIVE) {
	    (void) _wnMakeActive(win);
	}
	return _wnQRedraw(theWin, 0, 0, theWin->width, theWin->height);
    } else {
	return VEM_CANTFIND;
    }
}



vemStatus _wnQRedraw(theWin, x, y, width, height)
internalWin *theWin;		/* Real information about window             */
int x, y, width, height;	/* Region of window (in display coordinates) */
/*
 * This routine queues a request to redraw the specified physical region
 * of the specified OCT window.  These requests will be honored the
 * next time wnFlush is called.  This routine is used by wnFilter
 * to service ExposeRegion requests.
 */
{
    struct octBox octExtent;

    /* Check for a null box */
    if ((width <= 0) || (height <= 0)) {
	return VEM_BADARG;
    }

    /*
     * Translate physical region to an OCT region and queue.  Due to
     * integer rounding problems,  we must add a small amount of padding
     * to the region to make sure we get all of it.  If the scale factor
     * is positive (many oct units per display coordinate), the padding
     * is added to the physical region in this routine.  If the scale
     * factor is negative (many display units per oct coordinate),  the
     * padding is added to the OCT region in wnRedraw.
     */

    if (theWin->scaleFactor > 0) {
	x -= 1;  y -= 1;  width += 2;  height += 2;
    }

    octExtent.lowerLeft.x = theWin->fcts[W_PRI].x_off +
      OCTSCALE(x, theWin->scaleFactor);
    octExtent.lowerLeft.y = theWin->fcts[W_PRI].y_off +
      OCTSCALE(theWin->height - (y + height), theWin->scaleFactor);
    octExtent.upperRight.x = theWin->fcts[W_PRI].x_off +
      OCTSCALE(x + width, theWin->scaleFactor);
    octExtent.upperRight.y = theWin->fcts[W_PRI].y_off +
      OCTSCALE(theWin->height - y, theWin->scaleFactor);

    queueRedraw(theWin, &octExtent);
    return VEM_OK;
}


vemStatus wnForceRedraw(win, x, y, width, height)
Window win;			/* External window information     */
int x, y, width, height;	/* Region of window (disp. coords) */
/*
 * This is an external version of wnQRedraw which redraws a portion
 * of a window regardless of whether it is active or inactive.
 */
{
    internalWin *int_win;

    if (_wnFind(win, &int_win, (int *) 0, (int *) 0)) {
	return _wnQRedraw(int_win, x, y, width, height);
    } else {
	return VEM_CANTFIND;
    }
}



/*ARGSUSED*/
static enum st_retval searchWin(key, value, arg)
char *key;			/* Key of data item          */
char *value;			/* Value associated with key */
char *arg;			/* Standard argument         */
/*
 * This routine is used with the st_foreach routine to search through
 * the hash table of OCT windows.  We are interested in the value
 * (which is internalWin *) and the argument (which is facetExtent *).
 * If the window buffer matches the compare unit buffer,  the request
 * is queued.
 */
{
    internalWin *theWin = (internalWin *) value;
    bufferExtent *source = (bufferExtent *) arg;
    int idx;

    if (theWin->fcts[W_PRI].winFctId == source->searchFct) {
	queueRedraw(theWin, &(source->extent));
    }
    for (idx = 1;  idx < theWin->num_fcts;  idx++) {
	if (theWin->fcts[idx].winFctId == source->searchFct) {
	    /* Translate into primary extent */
	    source->extent.lowerLeft.x =
	      (source->extent.lowerLeft.x - theWin->fcts[idx].x_off) +
		theWin->fcts[W_PRI].x_off;
	    source->extent.lowerLeft.y =
	      (source->extent.lowerLeft.y - theWin->fcts[idx].y_off) +
		theWin->fcts[W_PRI].y_off;
	    source->extent.upperRight.x =
	      (source->extent.upperRight.x - theWin->fcts[idx].x_off) +
		theWin->fcts[W_PRI].x_off;
	    source->extent.upperRight.y =
	      (source->extent.upperRight.y - theWin->fcts[idx].y_off) +
		theWin->fcts[W_PRI].y_off;
	    queueRedraw(theWin, &(source->extent));
	}
    }
    return ST_CONTINUE;
}

static vemStatus instTrace(theInst, arg)
octObject *theInst;		/* Instance         */
char *arg;			/* User passed data */
/*
 * This routine is passed to the buffer module to carry out
 * a search across all instances of a buffer.  The routine
 * transforms the queued display region (passed in `arg') into
 * the instance's coordinate space and queues it recursively
 * using wnQRegion.  This effectively causes updates in
 * all of the places where a buffer is instantiated.
 */
{
    octObject instFct;
    struct octBox extent;
    octCoord temp;
    
    /* Find its facet */
    octGetFacet(theInst, &instFct);
    /* Translate extent into its coordinates */
    extent = *((struct octBox *) arg);
    tr_oct_transform(&(theInst->contents.instance.transform),
		     &extent.lowerLeft.x, &extent.lowerLeft.y);
    tr_oct_transform(&(theInst->contents.instance.transform),
		     &extent.upperRight.x, &extent.upperRight.y);
    VEMBOXNORM(extent, temp);

    /* Queue up this region for redraw */
#ifndef OLD_DISPLAY
    return wnQRedisplay(instFct.objectId, &extent);
#else
    return wnQRegion(instFct.objectId, &extent);
#endif
}

vemStatus wnQRegion(theFctId, theExtent)
octId theFctId;			/* Facet to redraw           */
struct octBox *theExtent;	/* Region of facet to redraw */
/*
 * The new vem automatically does display updates based on changes
 * detected using change lists.  These updates are done in the buffer
 * module.  This is a temporary replacement function for queueing
 * up changed regions that eventually will go away entirely.
 */
{
#ifndef OLD_DISPLAY
    octObject fct;

    fct.objectId = theFctId;
    if (octGetById(&fct) == OCT_OK) {
	vemMsg(MSG_L, "WARNING: forced redisplay on facet %s\n",
	       vuDispObject(&fct));
    } else {
	vemMsg(MSG_L, "WARNING: forced redisplay on facet [%d]\n",
	       theFctId);
    }
#else
    wnQRedisplay(theFctId, theExtent);
#endif
    return VEM_OK;
}

vemStatus wnQRedisplay(theFctId, theExtent)
octId theFctId;			/* Facet to redraw           */
struct octBox *theExtent;	/* Region of facet to redraw */
/*
 * This routine queues a request to redraw a specified virtual region
 * of the specified facet.  The region will be updated in ALL windows
 * where the region is visible.  Again,  note this routine simply
 * queues the request;  requests are not processed until a call
 * is made to wnFlush.
 */
{
    bufferExtent compareUnit;

    /* Set up a scan of the window hash table */

    compareUnit.searchFct = theFctId;
    compareUnit.extent = *theExtent;

    _wnForeach(WN_ACTIVE, searchWin, (char *) &compareUnit);

    /* Now handle updates to instances of this buffer */
    return bufInstances(theFctId, instTrace, (char *) theExtent);
}


/*ARGSUSED*/
static enum st_retval refrWin(key, value, arg)
char *key;			/* Key of data item          */
char *value;			/* Value associated with key */
char *arg;			/* Standard argument         */
/*
 * This routine is used with the st_foreach routine to cause
 * a redraw of all current VEM windows.
 */
{
    internalWin *theWin = (internalWin *) value;

    (void) _wnQRedraw(theWin, 0, 0, theWin->width, theWin->height);
    return ST_CONTINUE;
}


vemStatus wnRefresh()
/*
 * This routine causes a redraw of all VEM graphics windows.
 */
{
    _wnForeach(WN_ACTIVE, refrWin, (char *) 0);
    return VEM_OK;
}


static int queueRedraw(intWin, ext)
internalWin *intWin;		/* Internal window information */
struct octBox *ext;		/* OCT extent to redraw        */
/*
 * This routine queues a redraw event on the global redraw
 * queue "rd_queue".  The routine attempts to collapse regions
 * which are "close" to each other into one region to reduce
 * overall redraw time.  Some definitions:  the sum region of
 * two boxes is a region the sum of the widths wide and the sum
 * of the heights high.  The merge region of two boxes is the
 * smallest region which contains the two boxes.  Two boxes are
 * considered close if the width and height of the merged
 * region is less than twice the width and height of the sum region.
 * This is a O(n^2) algorithm but the number of things in the queue
 * should be fairly small.
 */
{
    int merges, index;
    octCoord mergeWidth, mergeHeight;
    octCoord sumWidth, sumHeight;

    do {
	merges = 0;
	for (index = 0;  index < rd_last;  index++) {
	    if (rd_queue[index].theWin == intWin) {
		/* Merging is possible */
		mergeWidth = VEMMAX(ext->upperRight.x,
				    rd_queue[index].extent.upperRight.x) -
			     VEMMIN(ext->lowerLeft.x,
				    rd_queue[index].extent.lowerLeft.x);
		sumWidth = (ext->upperRight.x - ext->lowerLeft.x) +
		  (rd_queue[index].extent.upperRight.x -
		   rd_queue[index].extent.lowerLeft.x);
		if (mergeWidth > (sumWidth+sumWidth)) {
		    /* Do not merge */
		    continue;
		}
		mergeHeight = VEMMAX(ext->upperRight.y,
				     rd_queue[index].extent.upperRight.y) -
			      VEMMIN(ext->lowerLeft.y,
				     rd_queue[index].extent.lowerLeft.y);
		sumHeight = (ext->upperRight.y - ext->lowerLeft.y) +
		  (rd_queue[index].extent.upperRight.y -
		   rd_queue[index].extent.lowerLeft.y);
		if (mergeHeight > (sumHeight + sumHeight)) {
		    /* Do not merge */
		    continue;
		}
		/*
		 * Merge the boxes.  We make the extent of the
		 * incoming box the merged box and remove the
		 * current box.  We test this one against the
		 * others so multiple mergings take place
		 */
		ext->lowerLeft.x = VEMMIN(ext->lowerLeft.x,
					  rd_queue[index].extent.lowerLeft.x);
		ext->lowerLeft.y = VEMMIN(ext->lowerLeft.y,
					  rd_queue[index].extent.lowerLeft.y);
		ext->upperRight.x = ext->lowerLeft.x + mergeWidth;
		ext->upperRight.y = ext->lowerLeft.y + mergeHeight;
		rd_queue[index] = rd_queue[rd_last-1];
		rd_last--;
		merges = 1;
		break;
	    }
	}
    } while (merges);
    /* Now add the box to the end (if no room,  flush the queue) */
    if (rd_last >= QUEUE_SIZE) {
	(void) wnQuickFlush();
    }
    rd_queue[rd_last].theWin = intWin;
    rd_queue[rd_last].extent = *ext;
    rd_last++;
    return 1;
}


vemStatus wnQSelect(objset, extent)
vemObjects *objset;		/* Selection set to redraw  */
struct octBox *extent;		/* Virtual region of buffer */
/*
 * This routine is similar to wnQRedisplay except only the objects
 * in the object set `objset' are redrawn.  This is used to
 * queue up redraws of selected items when the underlying selected
 * set is on its own plane.  Like wnQRedisplay,  these requests
 * are queued and will not be processed until a call to wnFlush().
 * The merging semantics are stolen directly from the queueing
 * strategy used for wnQRedisplay and wnQRedraw.  However,  unlike
 * those routines,  only virtual regions are merged here.
 */
{
    int merges, index;
    octCoord mergeWidth, mergeHeight;
    octCoord sumWidth, sumHeight;

    /* Make sure its not a null region */
    if ((extent->lowerLeft.x > extent->upperRight.x) ||
	(extent->lowerLeft.y > extent->upperRight.y))
      return VEM_BADARG;

    do {
	merges = 0;
	for (index = 0;  index < sel_last;  index++) {
	    if ((sel_queue[index].theFct == objset->theFct) &&
		(sel_queue[index].theAttr = objset->attr)) {
		/* Merging is possible */
		mergeWidth = VEMMAX(extent->upperRight.x,
				    sel_queue[index].extent.upperRight.x) -
			     VEMMIN(extent->lowerLeft.x,
				    sel_queue[index].extent.lowerLeft.x);
		sumWidth = (extent->upperRight.x - extent->lowerLeft.x) +
		  (sel_queue[index].extent.upperRight.x -
		   sel_queue[index].extent.lowerLeft.x);
		if (mergeWidth > (sumWidth+sumWidth)) {
		    /* Do not merge */
		    continue;
		}
		mergeHeight = VEMMAX(extent->upperRight.y,
				     sel_queue[index].extent.upperRight.y) -
			      VEMMIN(extent->lowerLeft.y,
				     sel_queue[index].extent.lowerLeft.y);
		sumHeight = (extent->upperRight.y - extent->lowerLeft.y) +
		  (sel_queue[index].extent.upperRight.y -
		   sel_queue[index].extent.lowerLeft.y);
		if (mergeHeight > (sumHeight + sumHeight)) {
		    /* Do not merge */
		    continue;
		}
		/*
		 * Merge the boxes.  We make the extent of the
		 * incoming box the merged box and remove the
		 * current box.  We test this one against the
		 * others so multiple mergings take place
		 */
		extent->lowerLeft.x = VEMMIN(extent->lowerLeft.x,
					  sel_queue[index].extent.lowerLeft.x);
		extent->lowerLeft.y = VEMMIN(extent->lowerLeft.y,
					  sel_queue[index].extent.lowerLeft.y);
		extent->upperRight.x = extent->lowerLeft.x + mergeWidth;
		extent->upperRight.y = extent->lowerLeft.y + mergeHeight;
		sel_queue[index] = sel_queue[sel_last-1];
		sel_last--;
		merges = 1;
		break;
	    }
	}
    } while (merges);
    /* Now add the box to the end (if no room,  flush the queue) */
    if (sel_last >= QUEUE_SIZE) {
	(void) wnQuickFlush();
    }
    sel_queue[sel_last].theFct = objset->theFct;
    sel_queue[sel_last].theAttr = objset->attr;
    sel_queue[sel_last].extent = *extent;
    sel_last++;
    return VEM_OK;
}



/* Not really an official module */
extern vemStatus vemBusy();


static int changes_should_be_processed = 1;
void   setImportantChangeFlag( f )
    int f;
{
    changes_should_be_processed = f;
}

#ifdef NEVER
static int processChanges()
/*
 * This function determines if there has been enough editing activity
 * to grant a processing of the change lists of all the buffers.
 * This turned out to be an extremely expensive operation, which 
 * we are now trying to use more judiciously.
 */
{
    return changes_should_be_processed;
}
#endif


vemStatus wnQuickFlush()
/*
 * This routine carries out all outstanding redraw requests.  This
 * should be called as a last resort when there is nothing else
 * left to do.  It is an expensive routine which may take some time
 * to complete.  The routine returns VEM_OK if all the redraws
 * completed and VEM_FALSE if any of the redraws did not complete
 * successfully.
 */
{
    vemStatus retCode = VEM_OK;

    if (rd_last > 0) {
	/* Flush normal requests */
	retCode = normFlush();
    }

    /* Flush selection on its own plane requests (if any) */
    if (sel_last > 0) retCode = VEMMIN(selFlush(), retCode);

    return retCode;
}


vemStatus wnFlush()
/*
 * This routine carries out all outstanding redraw requests.  This
 * should be called as a last resort when there is nothing else
 * left to do.  It is an expensive routine which may take some time
 * to complete.  The routine returns VEM_OK if all the redraws
 * completed and VEM_FALSE if any of the redraws did not complete
 * successfully.
 */
{
    /* Cause any change list operations to be added to queues */
    bufChanges();

    if ((rd_last > 0) || (sel_last > 0)) {
	vemBusy();
    }

    return wnQuickFlush();
}



static vemStatus normFlush()
/*
 * This routine flushes the normal redraw requests which have
 * been submitted by wnQRedisplay and wnQRedraw.  It handles
 * normal vs. alternate buffer redraws and will draw selection
 * and argument list objects as well.  For redraw of purely
 * selection information,  see selFlush().
 */
{
    register int index, fct_num;
    register internalWin *itemWin;
    int retCode = VEM_OK, flag;

    for (index = 0;  index < rd_last;  index++) {
	/* Does the window have an alternate buffer? */
	itemWin = rd_queue[index].theWin;
	/* Backward down list of facets */
	flag = 0;
	for (fct_num = itemWin->num_fcts-1;  fct_num >= 0;  fct_num--) {
	    if (fct_num > W_PRI) {
		rd_queue[index].extent.lowerLeft.x =
		  (rd_queue[index].extent.lowerLeft.x -
		   itemWin->fcts[W_PRI].x_off) +
		     itemWin->fcts[fct_num].x_off;
		rd_queue[index].extent.lowerLeft.y =
		  (rd_queue[index].extent.lowerLeft.y -
		   itemWin->fcts[W_PRI].y_off) +
		     itemWin->fcts[fct_num].y_off;
		rd_queue[index].extent.upperRight.x =
		  (rd_queue[index].extent.upperRight.x -
		   itemWin->fcts[W_PRI].x_off) +
		     itemWin->fcts[fct_num].x_off;
		rd_queue[index].extent.upperRight.y =
		  (rd_queue[index].extent.upperRight.y -
		   itemWin->fcts[W_PRI].y_off) +
		     itemWin->fcts[fct_num].y_off;
	    }
	    _wnRedraw(itemWin, &(rd_queue[index].extent), fct_num, flag);
	    flag |= DONT_CLR;
	}
    }
    rd_last = 0;
    return retCode;
}


/*ARGSUSED*/
static enum st_retval selCheck(key, value, arg)
char *key, *value, *arg;
/*
 * This routine is passed to st_foreach to check all of the active
 * windows for selection exposures.  It scans the entire queue
 * of selection regions and causes a redraw of those that intersect
 * the window.
 */
{
    internalWin *theWin = (internalWin *) value;
    register int idx;
    struct octBox physical, *virExtent;

    for (idx = 0;  idx < sel_last;  idx++) {
	if (sel_queue[idx].theFct == theWin->fcts[W_PRI].winFctId) {
	    /* Compute physical region */
	    virExtent = &(sel_queue[idx].extent);
	    physical.lowerLeft.x =
	      DISPSCALE(virExtent->lowerLeft.x - theWin->fcts[W_PRI].x_off,
			theWin->scaleFactor);
	    physical.lowerLeft.y = 1 + theWin->height -
	      DISPSCALE(virExtent->lowerLeft.y - theWin->fcts[W_PRI].y_off,
			theWin->scaleFactor);
	    physical.upperRight.x =
	      DISPSCALE(virExtent->upperRight.x - theWin->fcts[W_PRI].x_off,
			theWin->scaleFactor) + 1;
	    physical.upperRight.y = theWin->height -
	      DISPSCALE(virExtent->upperRight.y - theWin->fcts[W_PRI].y_off,
			theWin->scaleFactor);

	    /* Clip the physical region to the physical region of the window */

	    physical.lowerLeft.x = VEMMAX(physical.lowerLeft.x, 0);
	    physical.upperRight.x = VEMMIN(physical.upperRight.x, theWin->width);
	    physical.lowerLeft.y = VEMMIN(physical.lowerLeft.y, theWin->height);
	    physical.upperRight.y = VEMMAX(physical.upperRight.y, 0);

	    /* If the region is zero sized,  no redraw is done */
	    if ((physical.lowerLeft.x >= physical.upperRight.x) ||
		(physical.lowerLeft.y <= physical.upperRight.y))
	      continue;

	    /* Otherwise,  do redraw */
	    objSetRedraw(sel_queue[idx].theAttr, sel_queue[idx].theFct,
			 theWin, virExtent, &physical);
	}
    }
    return ST_CONTINUE;
}

static vemStatus selFlush()
/*
 * This routine flushes the selection only queue (sel_queue).  It
 * is used when selection is on its own plane to effect changes
 * only to that plane.  The routine simply scans the hash table
 * of windows and calls the selection refresh function as appropriate.
 */
{
    _wnForeach(WN_ACTIVE | WN_INTR, selCheck, (char *) 0);
    sel_last = 0;
    return VEM_OK;
}

